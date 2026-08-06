# Prototype Pattern — Class Diagram

Shows the four participants and their relationships. The `EmailCampaignTemplate`
(ConcretePrototype) *implements* the `Prototype` contract. The `PrototypeRegistry`
stores masters and hands out clones; the `CampaignService` (Client) creates new
objects only by cloning — never by calling `new` on the concrete class.

```mermaid
classDiagram
    class Prototype~T~ {
        <<interface>>
        +clone() T
    }

    class EmailCampaignTemplate {
        +name: string
        +subject: string
        +sender: Sender
        +tags: string[]
        +blocks: ContentBlock[]
        +metadata: Map~string,string~
        +updatedAt: Date
        +clone() EmailCampaignTemplate
        +describe() string
    }

    class PrototypeRegistry~T~ {
        -prototypes: Map~string,T~
        +register(key, prototype) void
        +create(key) T
        +keys() string[]
    }

    class CampaignService {
        -registry: PrototypeRegistry~EmailCampaignTemplate~
        +startCampaign(tenantKey, overrides) EmailCampaignTemplate
    }

    EmailCampaignTemplate ..|> Prototype : implements
    PrototypeRegistry --> Prototype : stores & clones
    CampaignService --> PrototypeRegistry : asks for clones
    CampaignService ..> EmailCampaignTemplate : receives clone
```

**How to read it**
- `..|>` (dashed, hollow triangle) = *implements interface* — `EmailCampaignTemplate` implements the `Prototype` contract.
- `-->` = *association / holds a reference*. The `PrototypeRegistry` holds prototypes; the `CampaignService` holds the registry.
- `..>` (dashed) = *depends on / uses* — the `CampaignService` receives a clone typed as `EmailCampaignTemplate`.
- The Client (`CampaignService`) never calls `new EmailCampaignTemplate(...)` directly; it asks the `PrototypeRegistry`, which returns clones. That is the whole point: creation is decoupled from the concrete class.
