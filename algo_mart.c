#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h> // Include conio.h for password masking (Windows specific)

#define TAX_RATE      0.02
#define MAX_PRODUCTS  20
#define CARD_LENGTH   16
#define FILENAME      "users.txt" // File to store usernames and passwords

void displayMenu(const char *products[], const float prices[]);
int getChoice();
float getQuantity();
void calculateTotal(float price, float quantity, float discount, float *totalPrice, float *taxAmount, float *discountAmount);
int processPayment(float totalPrice); // Updated function to return payment status
void clearScreen(); // Function to clear the screen based on OS
int userLogin(char *username); // Function to handle user login
void registerUser(); // Function to register a new user
void logPurchase(const char *username, const char *product, float quantity, float totalPrice, float taxAmount, float discountAmount); // Log purchase to history file
void viewHistory(const char *username); // View user purchase history
float applyLoyaltyDiscount(const char *username, float totalPrice); // Function to apply loyalty discount

int main() {
    char username[50];

    // User registration or login
    int option;
    printf("Welcome to Algo Mart!\n1. Login\n2. Register\n");
    scanf("%d", &option);

    if (option == 2) {
        registerUser();
    }

    if (!userLogin(username)) {
        printf("Login failed. Exiting the program.\n");
        return 0; // Exit the program if login fails
    }

    // Greet the user and present options
    clearScreen();
    printf("Welcome, %s!\n", username);

    const char *products[MAX_PRODUCTS] = {
        "Bread", "Milk", "Eggs", "Rice", "Sugar", "Salt", "Flour", "Corn Flour",
        "Apples", "Oranges", "Grapes", "Onions", "Tomatoes", "Potatoes",
        "Green Chillies", "Coriander (Dhania)", "Mint Leaves (Pudina)",
        "Air Freshener", "Floor Cleaner", "Bathroom Cleaner"
    };
    const float prices[MAX_PRODUCTS] = {120, 320, 340, 450, 170, 100, 600, 180,
                                        500, 780, 880, 120, 220, 100, 250, 20,
                                        20, 400, 700, 550};
    const float discounts[MAX_PRODUCTS] = {0.0, 0.03, 0.0, 0.05, 0.08, 0.02, 0.10,
                                           0.0, 0.03, 0.02, 0.02, 0.04, 0.03,
                                           0.0, 0.0, 0.0, 0.0, 0.03, 0.03, 0.04};

    while (1) {
        printf("\nSelect an option:\n1. Shop for Products\n2. View Purchase History\n3. Exit\n");
        int mainOption;
        scanf("%d", &mainOption);

        if (mainOption == 3) {
            printf("Thank you for visiting Algo Mart, %s! Goodbye!\n", username);
            break;
        }

        switch (mainOption) {
            case 1: { // Shopping
                displayMenu(products, prices);

                int choice = getChoice();
                if (choice < 1 || choice > MAX_PRODUCTS) {
                    printf("Invalid choice! Do you want to try again? (1 for Yes, 0 for No): ");
                    int retry;
                    scanf("%d", &retry);
                    if (retry == 0) {
                        printf("Returning to the main menu.\n");
                        break;
                    }
                    clearScreen(); // Clear the screen before retry
                    continue;
                }

                float quantity = getQuantity();
                if (quantity <= 0) {
                    printf("Invalid quantity! Do you want to try again? (1 for Yes, 0 for No): ");
                    int retry;
                    scanf("%d", &retry);
                    if (retry == 0) {
                        printf("Returning to the main menu.\n");
                        break;
                    }
                    clearScreen(); // Clear the screen before retry
                    continue;
                }

                float totalPrice = 0.0, taxAmount = 0.0, discountAmount = 0.0;
                calculateTotal(prices[choice - 1], quantity, discounts[choice - 1], &totalPrice, &taxAmount, &discountAmount);

                // Apply loyalty discounts
                float loyaltyDiscount = applyLoyaltyDiscount(username, totalPrice);
                totalPrice -= loyaltyDiscount;

                printf("Sales tax (%.2f%%) added: PKR %.2f\n", TAX_RATE * 100, taxAmount);
                printf("Product discount applied (%.2f%%): PKR %.2f\n", discounts[choice - 1] * 100, discountAmount);
                printf("Loyalty discount applied: PKR %.2f\n", loyaltyDiscount);
                printf("Total for %s (%.2f units): PKR %.2f\n", products[choice - 1], quantity, totalPrice);

                int paymentSuccess = processPayment(totalPrice);
                if (paymentSuccess) {
                    logPurchase(username, products[choice - 1], quantity, totalPrice, taxAmount, discountAmount);
                } else {
                    printf("Purchase not logged as payment failed.\n");
                }
                break;
            }
            case 2: { // View purchase history
                viewHistory(username);
                break;
            }
            default:
                printf("Invalid option! Please select a valid option.\n");
                break;
        }
    }
    return 0;
}

float applyLoyaltyDiscount(const char *username, float totalPrice) {
    char filename[100];
    sprintf(filename, "%s_history.txt", username); // Open the user's history file

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0.0; // No discount if no purchase history
    }

    float monthlyTotal = 0.0;
    int visitCount = 0;
    float overallTotal = 0.0;

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    int currentMonth = local->tm_mon + 1;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char product[50];
        float quantity, totalPrice, taxAmount, discountAmount;
        int day, month, year;

        sscanf(line, "Product: %[^,], Quantity: %f, Total Price: PKR %f, Sales Tax: PKR %f, Discount: PKR %f (Date: %d/%d/%d)",
               product, &quantity, &totalPrice, &taxAmount, &discountAmount, &day, &month, &year);

        overallTotal += totalPrice;
        if (month == currentMonth) {
            monthlyTotal += totalPrice;
            visitCount++;
        }
    }
    fclose(file);

    // Apply loyalty discounts
    float loyaltyDiscount = 0.0;
    if (overallTotal >= 10000) {
        loyaltyDiscount = totalPrice * 0.04; // 4% discount
    } else if (monthlyTotal >= 3000 && visitCount >= 3) {
        loyaltyDiscount = totalPrice * 0.05; // 5% discount
    } else if (overallTotal >= 5000) {
        loyaltyDiscount = totalPrice * 0.02; // 2% discount
    }

    return loyaltyDiscount;
}

void registerUser() {
    FILE *file = fopen(FILENAME, "a+");
    if (file == NULL) {
        printf("Error opening file for registration.\n");
        return;
    }

    char username[30], password[8], fileUsername[30], filePassword[8];
    int exists = 0;

    printf("Register a new user:\n");
    printf("Enter username: ");
    scanf("%s", username);

    // Check if username already exists
    while (fscanf(file, "%s %s", fileUsername, filePassword) != EOF) {
        if (strcmp(username, fileUsername) == 0) {
            exists = 1;
            break;
        }
    }

    if (exists) {
        printf("Username already exists! Try a different one.\n");
        fclose(file);
        return;
    }

    printf("Enter password: ");

    // Hide password input and show '*' characters
    int i = 0;
    while (1) {
        char ch = getch(); // Get a character without echoing
        if (ch == 13) { // Enter key pressed
            password[i] = '\0'; // End the string
            break;
        } else if (ch == 8 && i > 0) { // Backspace key pressed
            i--;
            printf("\b \b"); // Erase the '*' character
        } else {
            password[i] = ch;
            i++;
            printf("*"); // Print '*' for each character entered
        }
    }

    fprintf(file, "%s %s\n", username, password); // Save credentials to the file
    fclose(file);
    printf("\nRegistration successful! You can now log in.\n");
}

int userLogin(char *username) {
    char password[8];
    char fileUsername[30], filePassword[8];
    int attempts;

    while (1) { // Outer loop for retrying login
        attempts = 3; // Reset attempts for each retry
        while (attempts > 0) {
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");

            // Hide password input and show '*' characters
            int i = 0;
            while (1) {
                char ch = getch(); // Get a character without echoing
                if (ch == 13) { // Enter key pressed
                    password[i] = '\0'; // End the string
                    break;
                } else if (ch == 8 && i > 0) { // Backspace key pressed
                    i--;
                    printf("\b \b"); // Erase the '*' character
                } else {
                    password[i] = ch;
                    i++;
                    printf("*"); // Print '*' for each character entered
                }
            }

            FILE *file = fopen(FILENAME, "r");
            if (file == NULL) {
                printf("User database not found! No registered users.\n");
                printf("Do you want to register a new account? (1 for Yes, 0 for No): ");
                int registerOption;
                scanf("%d", &registerOption);
                if (registerOption == 1) {
                    registerUser();
                    continue; // Restart login process after registration
                } else {
                    printf("Returning to main menu.\n");
                    return 0;
                }
            }

            int loginSuccessful = 0;
            while (fscanf(file, "%s %s", fileUsername, filePassword) != EOF) {
                if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
                    loginSuccessful = 1;
                    break;
                }
            }
            fclose(file);

            if (loginSuccessful) {
                printf("\nLogin successful!\n");
                return 1; // Exit function if login is successful
            } else {
                attempts--;
                printf("\nIncorrect credentials. You have %d attempt(s) left.\n", attempts);
            }
        }

        // If attempts are exhausted, ask the user if they want to retry
        int retry;
        printf("Login failed. Do you want to retry? (1 for Yes, 0 for No): ");
        scanf("%d", &retry);
        if (retry == 0) {
            printf("Returning to main menu.\n");
            return 0; // Exit login process but not the program
        }
    }
}

void logPurchase(const char *username, const char *product, float quantity, float totalPrice, float taxAmount, float discountAmount) {
    char filename[100];
    sprintf(filename, "%s_history.txt", username);

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error logging purchase!\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    fprintf(file, "Product: %s, Quantity: %.2f, Total Price: PKR %.2f, Sales Tax: PKR %.2f, Discount: PKR %.2f (Date: %d/%d/%d)\n",
            product, quantity, totalPrice, taxAmount, discountAmount, local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);

    fclose(file);
}

void viewHistory(const char *username) {
    char filename[100];
    sprintf(filename, "%s_history.txt", username); // Open the user's history file

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No purchase history found.\n");
        return;
    }

    printf("\nPurchase History for %s:\n", username);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");  // For Windows systems
    #else
        system("clear"); // For Unix-based systems like Linux or macOS
    #endif
}

void displayMenu(const char *products[], const float prices[]) {
    printf("\nSelect a product:\n");
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        printf("%d. %s - PKR %.2f\n", i + 1, products[i], prices[i]);
    }
}

int getChoice() {
    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);
    return choice;
}

float getQuantity() {
    float quantity;
    printf("Enter the quantity: ");
    scanf("%f", &quantity);
    return quantity;
}

void calculateTotal(float price, float quantity, float discount, float *totalPrice, float *taxAmount, float *discountAmount) {
    *discountAmount = price * quantity * discount;
    *totalPrice = (price * quantity) - *discountAmount;
    *taxAmount = (*totalPrice) * TAX_RATE;
    *totalPrice += *taxAmount;
}

int processPayment(float totalPrice) {
    printf("\nThe total amount is: PKR %.2f\n", totalPrice);
    printf("Proceed to payment? (1 for Yes, 0 for No): ");
    int paymentOption;
    scanf("%d", &paymentOption);

    if (paymentOption == 1) {
        printf("Payment successful!\n");
        return 1; // Return success if payment is processed
    }
    return 0; // Return failure if payment is not processed
}
