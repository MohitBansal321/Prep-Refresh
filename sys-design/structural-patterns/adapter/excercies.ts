// Easy — Temperature Sensor Adapter

// Target interface that the client expects
interface Thermometer {
  getTemperatureCelsius(): number;
}

// Adaptee class that provides temperature in Fahrenheit
class FahrenheitSensor {
  readFahrenheit(): number { return 98.6; }
}

// Adapter class that implements the Thermometer interface and adapts the FahrenheitSensor
class FahrenheitSensorAdapter implements Thermometer {
    constructor(private sensor: FahrenheitSensor){}
    getTemperatureCelsius(): number {
        const fahrenheit = this.sensor.readFahrenheit();
        return (fahrenheit - 32) * 5/9;
    }
}


// Client code
function showTemperature (thermometer: Thermometer) {
    return thermometer.getTemperatureCelsius();
}

// Composition 
const sensor = new FahrenheitSensor();
const adapter = new FahrenheitSensorAdapter(sensor);
console.log(`Temperature in Celsius: ${showTemperature(adapter)}`);




// Medium — Unified Logger

// Target interface that the client expects
interface AppLogger {
  info(message: string): void;
  error(message: string): void;
}

// Adaptee class that provides logging in a different format (Third-party logger)
class WinstonLike {
  log(level: "info" | "error", msg: string): void { /* ... */ }
}
class ConsoleJsonLogger {
  write(payload: { severity: string; text: string }): void { /* ... */ }
}


// Adapter classes that implement the AppLogger interface and adapt the third-party loggers
class WinstonAdapter implements AppLogger {
    constructor(private logger: WinstonLike){}
    info(message: string): void {
        this.logger.log("info", message);
    }
    error(message: string): void {
        this.logger.log("error", message);
    }
}
class ConsoleJsonAdapter implements AppLogger {
    constructor(private logger: ConsoleJsonLogger){}
    info(message:string): void {
        this.logger.write({severity: "info", text: message});
    }
    error(message:string): void {
        this.logger.write({severity: "error", text: message});
    }
}


// Composition wrapping the third-party loggers with their respective adapters
const winstonLogger = new WinstonLike();
const winstonAdapter = new WinstonAdapter(winstonLogger);

const consoleJsonLogger = new ConsoleJsonLogger();
const consoleJsonAdapter = new ConsoleJsonAdapter(consoleJsonLogger);

// Client code using the unified AppLogger interface
function logInfo(logger: AppLogger, message: string) {
    logger.info(message);
}

function logError(logger: AppLogger, message: string) {
    logger.error(message);
}

// Using the adapters to log messages
logInfo(winstonAdapter, "This is an info message from Winston.");
logError(consoleJsonAdapter, "This is an error message from Console JSON Logger.");