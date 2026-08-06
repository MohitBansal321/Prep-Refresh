import { logger } from "../logger";

export class PaymentService {
  processPayment(amount: number): void {
    logger.info(`Payment processed: $${amount}`);
  }

  refundFailed(transactionId: string, reason: string): void {
    logger.error(`Refund failed for ${transactionId}: ${reason}`);
  }
}
