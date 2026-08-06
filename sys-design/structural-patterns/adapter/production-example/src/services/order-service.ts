import { logger } from "../logger";

export class OrderService {
  placeOrder(orderId: string): void {
    logger.info(`Order ${orderId} placed`);
  }

  cancelOrder(orderId: string): void {
    logger.warn(`Order ${orderId} cancelled`);
  }
}
