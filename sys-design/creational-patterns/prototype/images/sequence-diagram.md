# Prototype Pattern — Sequence Diagram

Shows the runtime message exchange: the expensive construction happens once at
startup, and every subsequent object is produced by a cheap in-memory `clone()`.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant Reg as PrototypeRegistry
    participant P as EmailCampaignTemplate (prototype)
    participant Svc as CampaignService (Client)

    Note over Root,P: Startup — build the master ONCE (DB + Redis cost paid here)
    Root->>P: new EmailCampaignTemplate({...brand defaults...})
    Root->>Reg: register("acme", prototype)

    Note over Svc,P: Runtime — create a new campaign (pure memory, no DB)
    Svc->>Reg: create("acme")
    activate Reg
    Reg->>P: clone()
    activate P
    Note over P: deep-copy tags, blocks, metadata(Map), updatedAt(Date)<br/>return a NEW EmailCampaignTemplate
    P-->>Reg: fresh independent clone
    deactivate P
    Reg-->>Svc: clone
    deactivate Reg

    Note over Svc: mutate the clone safely
    Svc->>Svc: campaign.name = "Summer Sale"; campaign.tags.push("summer")
    Note over P: prototype is UNTOUCHED — clone shares nothing with it
```

**How to read it**
- The **Composition Root** builds and registers the master exactly once — that is where the expensive DB/Redis assembly cost is paid.
- At runtime the Client only asks the registry for `create(key)`; the registry answers by calling `clone()`, never by returning the stored master.
- `clone()` deep-copies every reference type (`tags`, `blocks`, `metadata` Map, `updatedAt` Date) and returns a genuine new instance.
- Because the clone is deep and independent, the Client mutating it (bottom) never affects the stored prototype or any other clone.
