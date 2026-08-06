# Factory Method Pattern — Flow Diagram

Step-by-step control flow of a single creation request, showing both the Simple
Factory branch and the Factory Method (subclass) branch converging on the same
interface-typed result.

```mermaid
flowchart TD
    Start([Client needs to send a notification]) --> Ask["Client calls factory / creator<br/>(never calls new directly)"]
    Ask --> Which{Which form?}

    Which -- Simple Factory --> Switch["createSender(channel) looks up<br/>switch / registry map"]
    Switch --> Build1["Instantiate matching ConcreteProduct"]

    Which -- Factory Method --> Sub["ConcreteCreator.createSender()<br/>chosen by polymorphism"]
    Sub --> Build2["Subclass instantiates its ConcreteProduct"]

    Build1 --> Return["Return product typed as Product interface"]
    Build2 --> Return
    Return --> Use["Client / Creator uses product via interface"]
    Use --> End([Caller never knew the concrete class])
```

**Key idea:** whichever form you pick, the concrete `new` happens in exactly one
place and the result is always returned *typed as the Product interface*. The
Simple Factory decides via a *conditional/map on a key*; the Factory Method
decides via *subclass polymorphism*. The client on the far side is identical in
both — it uses the product through the interface and never learns the concrete class.
