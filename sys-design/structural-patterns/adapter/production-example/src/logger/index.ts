// Adapter wired ONCE — consumers just import `logger`
import { WinstonAdapter } from "./adapters";
import { WinstonLike } from "./adaptees";

export type { Logger } from "./contract";
export const logger = new WinstonAdapter(new WinstonLike());
