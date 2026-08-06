# Adapter Pattern — Sequence Diagram

Shows the runtime message exchange for one payment, including the composition-root
wiring and both success and error paths.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant C as PaymentService (Client)
    participant A as StripeAdapter (Adapter)
    participant S as Stripe SDK (Adaptee)

    Note over Root: Startup wiring (once)
    Root->>S: new StripeSDK()
    Root->>A: new StripeAdapter(sdk)
    Root->>C: new PaymentService(adapter as PaymentGateway)

    Note over C,S: Runtime — one checkout
    C->>A: pay(5000, "USD", "tok_visa_4242")
    activate A
    Note over A: translate args → Stripe shape<br/>(currency→"usd")
    A->>S: charges.create({amount:5000, currency:"usd", source:"tok_visa_4242"})
    activate S
    S-->>A: { id:"ch_4242", status:"succeeded" }
    deactivate S
    Note over A: map result → PaymentResult
    A-->>C: { transactionId:"ch_4242", success:true, provider:"stripe" }
    deactivate A

    alt Vendor call fails
        C->>A: pay(...)
        activate A
        A->>S: charges.create(...)
        activate S
        S-->>A: throws StripeError
        deactivate S
        Note over A: translate error → PaymentError
        A-->>C: throws PaymentError("Stripe charge failed")
        deactivate A
    end
```

**How to read it**
- The **Composition Root** creates the Adaptee, wraps it in the Adapter, and injects the Adapter into the Client *as the Target type* — done once at startup.
- At runtime the Client only ever talks to the **Adapter**; it never references the SDK.
- The Adapter is the only participant that knows Stripe's method names, casing rules, data shapes, and error types.
- The `alt` block shows error translation: vendor exceptions become our own `PaymentError` before reaching the Client.
