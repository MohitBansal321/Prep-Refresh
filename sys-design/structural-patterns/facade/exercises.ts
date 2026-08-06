//  Easy — Home Theater Facade

class Amplifier {
  on(): void {}
  off(): void {}
  setVolume(level: number): void {
    console.log(`Amplifier volume set to ${level}`);
  }
}
class DvdPlayer {
  on(): void {}
  off(): void {}
  play(movie: string): void {
    console.log(`Playing movie: ${movie}`);
  }
  stop(): void {}
}
class Projector {
  on(): void {}
  off(): void {}
  wideScreenMode(): void {}
}
class Lights {
  dim(percent: number): void {
    console.log(`Lights dimmed to ${percent}%`);
  }
  on(): void {}
}
class Screens {
  down(): void {
    console.log("Screens down");
  }
  up(): void {
    console.log("Screens up");
  }
}

class HomeTheaterFacade {
  constructor(
    private amp: Amplifier,
    private dvd: DvdPlayer,
    private projector: Projector,
    private lights: Lights,
    private screens: Screens,
  ) {}

  watchMovie(movie: string): void {
    console.log("Get ready to watch a movie...");
    this.lights.dim(10);
    this.screens.down();
    this.projector.on();
    this.projector.wideScreenMode();
    this.amp.on();
    this.amp.setVolume(5);
    this.dvd.on();
    this.dvd.play(movie);
    console.log(`Now playing: ${movie}`);
  }

  endMovie(): void {
    console.log("Shutting down the home theater...");
    this.dvd.stop();
    this.dvd.off();
    this.amp.off();
    this.projector.off();
    this.screens.up();
    this.lights.on();
  }
}

const amp = new Amplifier();

const facade = new HomeTheaterFacade(
  amp,
  new DvdPlayer(),
  new Projector(),
  new Lights(),
  new Screens(),
);

facade.watchMovie("Inception");
facade.endMovie();
amp.setVolume(10); // Directly interacting with the Amplifier, bypassing the facade

//  Medium Onboarding Facade with Rollback

class AuthService {
  createAccount(email: string): Promise<{ userId: string }>{
        try {
            if(!email) {
                throw new Error("Invalid email address");
            }
            console.log(`Creating account for ${email}`);
            // Special test email to force a downstream BILLING failure so the rollback path can be exercised.
            const userId = email === "fail-billing@example.com" ? "invalid-user" : "user123";
            return Promise.resolve({ userId });
        } catch (error) {
            console.error("Error creating account:", error);
            return Promise.reject(error);
        }  
   }

  deleteAccount(userId: string): Promise<void>{
    try {
        console.log(`Deleting account for ${userId}`);
        return Promise.resolve();
    } catch (error) {
        console.error("Error deleting account:", error);
        return Promise.reject(error);
    }
  }

}
class BillingService {
  startTrial(userId: string): Promise<{ subscriptionId: string }>{
    try{
        if(!userId || userId === "invalid-user") {
            throw new Error("Invalid user ID");
        }
        console.log(`Starting trial for user: ${userId}`);
        return Promise.resolve({ subscriptionId: "sub123" });
    } catch (error) {
        console.error("Error starting trial:", error);
        return Promise.reject(error);   
    }
  }
}

class EmailService {
  sendWelcome(email: string): Promise<void>{
    try {
      console.log(`Sending welcome email to: ${email}`);
      return Promise.resolve();
    } catch (error) {
      console.error("Error sending welcome email:", error);
      return Promise.reject(error);
    }
  }
}


export class OnboardingError extends Error {
    constructor(message: string,
        public readonly stage: "AUTH" | "BILLING" | "EMAIL", 
        public readonly cause?: unknown) 
        {
    super(message);
    this.name = "OnboardingError";
  }
}

class OnboardingFacade {
  constructor(
    private authService: AuthService,
    private billingService: BillingService,
    private emailService: EmailService,
  ) {}

  async register(email:string): Promise<{ userId: string, subscriptionId: string }> {
    let userId: string | undefined;

    try{
        const authResult = await this.authService.createAccount(email);
        userId = authResult.userId;
    }
    catch (error) {
        throw new OnboardingError("Failed to create account", "AUTH", error);
    }

    let subscriptionId: string | undefined;
    try {
        const billingResult = await this.billingService.startTrial(userId);
        subscriptionId = billingResult.subscriptionId;
    }
    catch (error) {
        if (userId) {
            await this.authService.deleteAccount(userId);
        }
        throw new OnboardingError("Failed to start trial", "BILLING", error);
    }

    try {
        await this.emailService.sendWelcome(email);
    }
    catch (error) {
        console.warn("Failed to send welcome email, but proceeding with onboarding.");
    }
    
    console.log("Onboarding completed successfully.");
    return { userId, subscriptionId };
    
  }
}
const onBoardingUser = new OnboardingFacade(new AuthService(), new BillingService(), new EmailService());

async function runOnboardingTests() {
    // 1. Happy path — everything succeeds.
    console.log("--- Test: happy path ---");
    const result = await onBoardingUser.register("user@example.com");
    console.log("Result:", result);

    // 2. AUTH failure — empty email, no account was ever created, so no rollback needed.
    console.log("\n--- Test: AUTH failure ---");
    try {
        await onBoardingUser.register("");
    } catch (error) {
        if (error instanceof OnboardingError) {
            console.error(`Onboarding failed at stage: ${error.stage}`, error.cause);
        }
    }

    // 3. BILLING failure — account gets created, then startTrial rejects,
    //    so the facade must call authService.deleteAccount(userId) before rethrowing.
    console.log("\n--- Test: BILLING failure + rollback ---");
    try {
        await onBoardingUser.register("fail-billing@example.com");
    } catch (error) {
        if (error instanceof OnboardingError) {
            console.error(`Onboarding failed at stage: ${error.stage}`, error.cause);
        }
    }
}

runOnboardingTests();