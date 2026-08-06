import { Logger } from "./contract";
import { WinstonLike, ConsoleJsonLogger } from "./adaptees";

export class WinstonAdapter implements Logger {
  constructor(private adaptee: WinstonLike) {}

  info(message: string, meta?: Record<string, unknown>): void {
    this.adaptee.log("info", message, meta);
  }

  error(message: string, meta?: Record<string, unknown>): void {
    this.adaptee.log("error", message, meta);
  }

  warn(message: string, meta?: Record<string, unknown>): void {
    this.adaptee.log("warn", message, meta);
  }
}

export class ConsoleJsonAdapter implements Logger {
  constructor(private adaptee: ConsoleJsonLogger) {}

  info(message: string, meta?: Record<string, unknown>): void {
    this.adaptee.write({ severity: "info", text: message, data: meta });
  }

  error(message: string, meta?: Record<string, unknown>): void {
    this.adaptee.write({ severity: "error", text: message, data: meta });
  }

  warn(message: string, meta?: Record<string, unknown>): void {
    this.adaptee.write({ severity: "warn", text: message, data: meta });
  }
}
