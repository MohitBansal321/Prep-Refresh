# Abstract Factory Pattern — Sequence Diagram

Shows the runtime message exchange for one event, including the composition-root
wiring where the family is chosen once and the client builds its whole family up front.

```mermaid
sequenceDiagram
    autonumber
    participant Root as Composition Root
    participant F as AwsFactory (ConcreteFactory)
    participant C as EventPipelineService (Client)
    participant P as Products (S3/SQS/RDS)

    Note over Root: Startup — choose family from config (once)
    Root->>F: new AwsFactory(config)
    Root->>C: new EventPipelineService(factory as CloudResourceFactory)

    Note over C,P: Client builds its family up front
    C->>F: createBlobStorage()
    F-->>C: S3Storage (as BlobStorage)
    C->>F: createMessageQueue()
    F-->>C: SqsQueue (as MessageQueue)
    C->>F: createDatabase()
    F-->>C: RdsDatabase (as SqlDatabase)

    Note over C,P: Runtime — process one event
    C->>P: db.connect()
    C->>P: storage.put("events/evt-1001", ...)
    P-->>C: { url: "s3://..." }
    C->>P: db.query("INSERT ...")
    C->>P: queue.publish("events.ingested", "evt-1001")
    P-->>C: { messageId }
    Note over C: All products share the same family/config by construction
```

**How to read it**
- The **Composition Root** chooses the family once, builds the matching Concrete Factory, and injects it into the Client *as the abstract `CloudResourceFactory` type* — the client never sees `AwsFactory`.
- The Client asks the factory for each product and receives them typed as abstract products (`BlobStorage`, `MessageQueue`, `SqlDatabase`) — it never `new`s a concrete class.
- Because all three products came from the *same* factory, they share provider, region, and credentials by construction — you cannot pair an AWS queue with a GCP bucket.
- To run on GCP, only the first step changes (`new GcpFactory(config)`); every message from the Client onward is identical.
