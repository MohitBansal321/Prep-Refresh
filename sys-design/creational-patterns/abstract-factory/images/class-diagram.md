# Abstract Factory Pattern — Class Diagram

Shows the five participants and their relationships. The Client depends only on the
Abstract Factory interface and the Abstract Product interfaces. Each Concrete Factory
*implements* the Abstract Factory and *creates* only its own family's Concrete Products.

```mermaid
classDiagram
    class CloudResourceFactory {
        <<interface>>
        +createBlobStorage() BlobStorage
        +createMessageQueue() MessageQueue
        +createDatabase() SqlDatabase
    }

    class BlobStorage {
        <<interface>>
        +put(key, data, contentType) url
        +get(key) Buffer
    }
    class MessageQueue {
        <<interface>>
        +publish(topic, message) messageId
        +consume(topic) string
    }
    class SqlDatabase {
        <<interface>>
        +connect() void
        +query(sql, params) rows
    }

    class AwsFactory {
        +createBlobStorage() BlobStorage
        +createMessageQueue() MessageQueue
        +createDatabase() SqlDatabase
    }
    class GcpFactory {
        +createBlobStorage() BlobStorage
        +createMessageQueue() MessageQueue
        +createDatabase() SqlDatabase
    }

    class S3Storage
    class SqsQueue
    class RdsDatabase
    class GcsStorage
    class PubSubQueue
    class CloudSqlDatabase

    class EventPipelineService {
        -factory: CloudResourceFactory
        +process(event) void
    }

    AwsFactory ..|> CloudResourceFactory : implements
    GcpFactory ..|> CloudResourceFactory : implements

    S3Storage ..|> BlobStorage
    GcsStorage ..|> BlobStorage
    SqsQueue ..|> MessageQueue
    PubSubQueue ..|> MessageQueue
    RdsDatabase ..|> SqlDatabase
    CloudSqlDatabase ..|> SqlDatabase

    AwsFactory ..> S3Storage : creates
    AwsFactory ..> SqsQueue : creates
    AwsFactory ..> RdsDatabase : creates
    GcpFactory ..> GcsStorage : creates
    GcpFactory ..> PubSubQueue : creates
    GcpFactory ..> CloudSqlDatabase : creates

    EventPipelineService --> CloudResourceFactory : depends on
    EventPipelineService --> BlobStorage : uses
    EventPipelineService --> MessageQueue : uses
    EventPipelineService --> SqlDatabase : uses
```

**How to read it**
- `..|>` (dashed, hollow triangle) = *implements interface*. Both factories implement `CloudResourceFactory`; each concrete product implements one abstract product.
- `..>` (dashed arrow) = *creates / depends on*. Each factory creates only its own family's products — `AwsFactory` never points at a GCP product. That is what makes mixing families structurally impossible.
- `-->` = *association / holds a reference*. The Client (`EventPipelineService`) holds the factory and uses products **only through the abstract interfaces** — it has no arrow to any concrete class, so it never knows which cloud it runs on.
