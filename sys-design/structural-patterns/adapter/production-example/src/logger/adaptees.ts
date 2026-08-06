// Third-party SDKs — we don't control these
export class WinstonLike {
  log(level: "info" | "error" | "warn", msg: string, context?: object): void {
    console.log(`[WinstonLike] ${level}: ${msg}`, context ?? "");
  }
}

export class ConsoleJsonLogger {
  write(payload: { severity: string; text: string; data?: object }): void {
    console.log(JSON.stringify(payload));
  }
}
