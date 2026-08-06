# Adapter Pattern — Class Diagram

Shows the four participants and their relationships. The Client depends only on the
Target interface. Each Adapter *implements* the Target and *wraps* (composition) a
concrete Adaptee.

```mermaid
classDiagram
    class PaymentGateway {
        <<interface>>
        +pay(amountInCents, currency, cardToken) PaymentResult
        +refund(transactionId) RefundResult
    }

    class PaymentService {
        -gateway: PaymentGateway
        +checkout(order) void
    }

    class StripeAdapter {
        -stripe: StripeSDK
        +pay(amountInCents, currency, cardToken) PaymentResult
        +refund(transactionId) RefundResult
    }

    class RazorpayAdapter {
        -razorpay: RazorpaySDK
        +pay(amountInCents, currency, cardToken) PaymentResult
        +refund(transactionId) RefundResult
    }

    class StripeSDK {
        +charges: ChargesApi
        +refunds: RefundsApi
    }

    class RazorpaySDK {
        +createPayment(paise, currency, token)
        +issueRefund(paymentId)
    }

    PaymentService --> PaymentGateway : depends on (Target)
    StripeAdapter ..|> PaymentGateway : implements
    RazorpayAdapter ..|> PaymentGateway : implements
    StripeAdapter --> StripeSDK : wraps (Adaptee)
    RazorpayAdapter --> RazorpaySDK : wraps (Adaptee)
```

**How to read it**
- `..|>` (dashed, hollow triangle) = *implements interface*. Both adapters implement `PaymentGateway`.
- `-->` = *association / holds a reference*. `PaymentService` holds a `PaymentGateway`; each adapter holds its SDK.
- The Client (`PaymentService`) has **no arrow to any SDK** — that is the whole point: it is decoupled from every vendor.
