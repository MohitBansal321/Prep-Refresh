import { logger } from "../logger";

export class NotificationService {
  sendEmail(to: string, subject: string): void {
    logger.info(`Email sent to ${to}: ${subject}`);
  }
}
