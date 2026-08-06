# Adapter Pattern — Flow Diagram

Step-by-step control flow of a single `pay()` call through the adapter, plus the
error-translation branch.

```mermaid
flowchart TD
    Start([Client needs to charge a card]) --> Call["Client calls gateway.pay(amount, currency, token)"]
    Call --> Adapter{{"Adapter receives the call"}}
    Adapter --> Translate["Translate arguments into vendor's format<br/>(units, casing, object shape)"]
    Translate --> Invoke["Invoke Adaptee method<br/>(e.g. stripe.charges.create)"]
    Invoke --> Ok{"Adaptee call<br/>succeeded?"}
    Ok -- Yes --> MapBack["Map vendor result → PaymentResult<br/>(our domain type)"]
    Ok -- No --> MapErr["Wrap vendor error → PaymentError<br/>(our domain error)"]
    MapBack --> Return["Return PaymentResult to Client"]
    MapErr --> Throw["Throw PaymentError to Client"]
    Return --> End([Client continues, unaware of the vendor])
    Throw --> End
```

**Key idea:** every box between "Adapter receives the call" and "Return/Throw" is
*translation only*. No business rules live here — they stay in the Client. Swapping
the vendor changes only what happens inside `Translate` / `Invoke` / `MapBack`.
