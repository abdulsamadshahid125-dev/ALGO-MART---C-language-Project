# Algo Mart - Shopping Management System

Algo Mart is a feature-rich, command-line shopping management system built entirely in C. It simulates a real-world supermarket checkout experience, complete with user authentication, product cataloging, automated tax/discount calculations, and a customer loyalty program backed by file handling.

## Features

 User Authentication:  Secure User Registration and Login system.
  - Passwords are masked with `*` during input for enhanced security (Windows-specific).
 Product Catalog:   Built-in inventory of 20 everyday grocery products with pre-assigned prices and individual discount rates.

 Smart Checkout System:   Calculates gross total based on user-defined quantities.
  - Automatically applies a flat "2% Sales Tax".
  - Deducts product-specific discounts.

Customer Loyalty Program:  Analyzes past purchase history to apply dynamic loyalty discounts:
  -  5% Discount: If monthly total >= PKR 3000 and visits >= 3.
  -  4% Discount: If overall historical spending >= PKR 10000.
  -  2% Discount: If overall historical spending >= PKR 5000.

Persistent Storage (File Handling):
  - User credentials are saved in `users.txt`.
  - Individual purchase histories are logged in personalized files (e.g., `username_history.txt`) with dates and precise transaction breakdowns.

Cross-Platform Screen Clearing: Automatically detects the OS to clear the terminal screen for a clean UI experience.

## Technology Stack

Language: C

Core Libraries: `<stdio.h>`, `<stdlib.h>`, `<string.h>` (Standard I/O and utilities)
  - `<time.h>` (To track dates for monthly loyalty discounts)
  - `<conio.h>` (For password masking via `getch()`)

## How to Run

Prerequisites: You need a C compiler (like GCC) installed on your system. 'Note: Because this project uses `<conio.h>` for password masking, it is best compiled and run on a Windows environment.'

## Clone the repository:
   ```bash
   git clone [https://github.com/abdulsamadshahid125-dev/algo-mart.git](https://github.com/abdulsamadshahid125-dev/algo-mart.git)


Author: Abdul Samad Shahid
