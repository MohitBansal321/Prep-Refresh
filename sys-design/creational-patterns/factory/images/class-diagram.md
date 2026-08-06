# Factory Method Pattern — Class Diagram

Shows the four participants. The Client/Creator depends only on the Product
interface (`NotificationSender`). Each ConcreteCreator *extends* the Creator and
overrides the factory method to *instantiate* one ConcreteProduct.

```mermaid
classDiagram
    class NotificationSender {
        <<interface>>
        +channel: string
        +send(to, subject, body) SendResult
    }

    class EmailSender {
        -config: EmailConfig
        +send(to, subject, body) SendResult
    }
    class SmsSender {
        -config: SmsConfig
        +send(to, subject, body) SendResult
    }
    class PushSender {
        -config: PushConfig
        +send(to, subject, body) SendResult
    }

    class NotificationDispatcher {
        <<abstract>>
        #createSender()* NotificationSender
        +dispatch(request) SendResult
    }
    class EmailDispatcher {
        -config: EmailConfig
        #createSender() NotificationSender
    }
    class SmsDispatcher {
        -config: SmsConfig
        #createSender() NotificationSender
    }

    EmailSender ..|> NotificationSender : implements
    SmsSender ..|> NotificationSender : implements
    PushSender ..|> NotificationSender : implements

    EmailDispatcher --|> NotificationDispatcher : extends
    SmsDispatcher --|> NotificationDispatcher : extends

    NotificationDispatcher ..> NotificationSender : creates (factory method)
    EmailDispatcher ..> EmailSender : instantiates
    SmsDispatcher ..> SmsSender : instantiates
```

**How to read it**
- `#createSender()*` marks the abstract factory method (`*` = abstract, `#` = protected).
- `..|>` (dashed, hollow triangle) = *implements interface*. Each ConcreteProduct implements `NotificationSender`.
- `--|>` (solid, hollow triangle) = *extends / inheritance*. Each ConcreteCreator extends `NotificationDispatcher`.
- `..>` (dashed arrow) = *creates / instantiates*. Note the Creator points at the **Product interface**, while each ConcreteCreator points at the specific **ConcreteProduct** it builds.
- The Creator has **no arrow to any concrete product** — it depends only on the interface. That decoupling is the whole point.
