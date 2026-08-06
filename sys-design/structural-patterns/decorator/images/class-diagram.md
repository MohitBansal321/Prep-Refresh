# Decorator Pattern — Class Diagram

Shows the four participants. Both the Concrete Component and the abstract Decorator
implement the **same** Component interface (`HttpClient`). The abstract Decorator also
*holds* an `HttpClient` (the `inner` field) — that combination of "implements the
interface" **and** "holds the interface" is what lets decorators wrap each other and
stack recursively. Each Concrete Decorator adds exactly one responsibility.

```mermaid
classDiagram
    class HttpClient {
        <<interface>>
        +send(request) Promise~HttpResponse~
    }

    class FetchHttpClient {
        +networkCalls: number
        +send(request) Promise~HttpResponse~
    }

    class HttpClientDecorator {
        <<abstract>>
        #inner: HttpClient
        +send(request) Promise~HttpResponse~
    }

    class LoggingHttpClient {
        -logger: Logger
        +send(request) Promise~HttpResponse~
    }

    class RetryHttpClient {
        -opts: RetryOptions
        +send(request) Promise~HttpResponse~
    }

    class CacheHttpClient {
        -store: CacheStore
        +send(request) Promise~HttpResponse~
    }

    class RateLimitHttpClient {
        -tokens: number
        +send(request) Promise~HttpResponse~
    }

    class UserApiClient {
        -http: HttpClient
        +getUser(id) Promise~HttpResponse~
    }

    HttpClient <|.. FetchHttpClient : implements
    HttpClient <|.. HttpClientDecorator : implements
    HttpClientDecorator <|-- LoggingHttpClient : extends
    HttpClientDecorator <|-- RetryHttpClient : extends
    HttpClientDecorator <|-- CacheHttpClient : extends
    HttpClientDecorator <|-- RateLimitHttpClient : extends
    HttpClientDecorator o--> HttpClient : wraps (inner)
    UserApiClient --> HttpClient : depends on
```

**How to read it**
- `<|..` (dashed, hollow triangle) = *implements interface*. Both `FetchHttpClient` and the abstract `HttpClientDecorator` implement `HttpClient`.
- `<|--` (solid, hollow triangle) = *extends class*. Each concrete decorator extends the abstract `HttpClientDecorator`.
- `o-->` (hollow diamond) = *aggregation / holds a reference*. The decorator holds an `HttpClient` as its `inner` — and because that field is typed as the **interface**, `inner` can itself be another decorator. That is the recursion that makes stacking possible.
- `UserApiClient` (the Client) has an arrow only to the `HttpClient` **interface** — never to any concrete decorator or to `FetchHttpClient`. That is why the client cannot tell how many layers exist.
