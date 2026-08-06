# Observer Pattern — Class Diagram

Shows the five roles. The `OrderService` (ConcreteSubject) knows its observers
ONLY through the `OrderObserver` interface — it has no reference to any concrete
observer type. Each concrete observer implements `OrderObserver` and depends on
its own injected collaborator (mailer, repository, etc.).

```mermaid
classDiagram
    class OrderSubject {
        <<interface>>
        +subscribe(o: OrderObserver) void
        +unsubscribe(o: OrderObserver) void
        +notify(e: OrderPlacedEvent) Promise
    }

    class OrderObserver {
        <<interface>>
        +name: string
        +update(e: OrderPlacedEvent) Promise
    }

    class AbstractOrderSubject {
        -observers: Set~OrderObserver~
        +subscribe(o) void
        +unsubscribe(o) void
        +notify(e) Promise
    }

    class OrderService {
        +placeOrder(order) Promise
    }

    class EmailObserver {
        -mailer: Mailer
        +update(e) Promise
    }
    class InventoryObserver {
        -inventory: InventoryRepository
        +update(e) Promise
    }
    class AnalyticsObserver {
        -analytics: AnalyticsClient
        +update(e) Promise
    }
    class AuditLogObserver {
        -audit: AuditLogRepository
        +update(e) Promise
    }

    AbstractOrderSubject ..|> OrderSubject : implements
    OrderService --|> AbstractOrderSubject : extends
    AbstractOrderSubject o-- OrderObserver : holds many (1..*)

    EmailObserver ..|> OrderObserver : implements
    InventoryObserver ..|> OrderObserver : implements
    AnalyticsObserver ..|> OrderObserver : implements
    AuditLogObserver ..|> OrderObserver : implements
```

**How to read it**
- `<<interface>>` = a contract, no implementation.
- `..|>` (dashed, hollow triangle) = *implements an interface*.
- `--|>` (solid, hollow triangle) = *extends a class* (`OrderService` reuses the
  subscribe/notify machinery from `AbstractOrderSubject`).
- `o--` (hollow diamond) = *aggregation*: the subject **holds a collection of**
  observers. The `1..*` means one subject, many observers — the defining
  one-to-many dependency of this pattern.
- Notice there is **no arrow from `OrderService` to any concrete observer**. That
  absence is the whole point: the subject is decoupled from who listens.
