# Facade Pattern — Sequence Diagram

Shows the runtime message exchange for one checkout, including the composition-root
wiring, the happy-path fan-out to four subsystems, and the compensating-rollback
path when payment fails.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant C as OrderController (Client)
    participant F as OrderCheckoutFacade (Facade)
    participant I as InventoryService
    participant P as PaymentService
    participant S as ShippingService
    participant N as NotificationService

    Note over Root: Startup wiring (once)
    Root->>I: new InventoryService()
    Root->>P: new PaymentService()
    Root->>S: new ShippingService()
    Root->>N: new NotificationService()
    Root->>F: new OrderCheckoutFacade(inventory, payment, shipping, notification)
    Root->>C: new OrderController(facade)

    Note over C,N: Runtime — one successful checkout
    C->>F: placeOrder(request)
    activate F
    F->>I: reserve(lines)
    I-->>F: { reservationId }
    F->>P: charge(amount, currency, token)
    P-->>F: { transactionId }
    F->>S: createShipment(orderId, address)
    S-->>F: { trackingNumber }
    F->>N: sendOrderConfirmation(email, orderId, tracking)
    N-->>F: ok (best-effort)
    F-->>C: { status:"CONFIRMED", transactionId, trackingNumber }
    deactivate F

    alt Payment fails (compensating rollback)
        C->>F: placeOrder(request)
        activate F
        F->>I: reserve(lines)
        I-->>F: { reservationId }
        F->>P: charge(...)
        P-->>F: throws CardDeclinedError
        Note over F: compensate
        F->>I: release(reservationId)
        F-->>C: throws CheckoutError(stage=PAYMENT)
        deactivate F
    end
```

**How to read it**
- The **Composition Root** creates the four subsystems, injects them into the **Facade**, and injects the Facade into the **Client** — done once at startup (in NestJS this is the DI container).
- At runtime the Client sends exactly **one** message (`placeOrder`); the Facade fans it out into several ordered subsystem calls. One client call → many subsystem calls is the visible signature of a Facade.
- The Facade is the only participant that knows the *order* of calls and the *rollback* rules. The subsystems never message each other.
- The `alt` block shows the compensating action: when payment throws, the Facade releases the earlier reservation and surfaces a single `CheckoutError` (tagged with the stage) — the client never sees the subsystem-specific `CardDeclinedError`.
