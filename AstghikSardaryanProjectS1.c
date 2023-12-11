#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to check if a string is a valid barcode with exactly 12 digits
int isValidBarcode(const char* barcode) {
    if (strlen(barcode) != 12) {
        return 0;  // Not exactly 12 digits
    }

    for (int i = 0; i < 12; i++) {
        if (!isdigit(barcode[i])) {
            return 0;  // Contains a non-digit character
        }
    }

    return 1;  // Valid barcode
}

// Function to check if a string is a valid date
int isValidDate(const char* date) {
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) == 3) {
        if (year >= 1950 && year <= 2023) {
            if (month >= 1 && month <= 12) {
                if ((day >= 1 && day <= 31) &&
                    ((month == 4 || month == 6 || month == 9 || month == 11) ? day <= 30 : 1) &&
                    ((month == 2) ? (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0) ? day <= 29 : day <= 28) : 1)) {
                    return 1;  // Valid date
                }
            }
        }
    }
    return 0;  // Invalid date
}

// Function to check if a string is a valid numeric value
int isValidNumeric(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;  // Contains a non-digit character
        }
    }
    return 1;  // Valid numeric value
}

// Structure to represent a product
struct Product {
    char barcode[13];  // Increase the size by 1 to accommodate null terminator
    char name[100];
    char description[100];
    double price;
    int quantity;
    char supplier[100];
};

// Structure to represent a sale
struct Sale {
    char barcode[13]; 
    int quantitySold;
    char date[20];
    char customer[100];
};

// Dynamic arrays to store products and sales 
struct Product* products = NULL;
struct Sale* sales = NULL;
int productCount = 0;
int saleCount = 0;

// Function to add a product
void addProduct() {
    struct Product product;
    // An infinite loop to check if the barcode is valid
    while (1) {
        printf("Enter product barcode (exactly 12 digits): ");
        scanf(" %[^\n]", product.barcode);

        if (isValidBarcode(product.barcode)) {
            break;
        }
        else {
            printf("Invalid barcode. Please enter a valid 12-digit barcode.\n");
        }
    }
    printf("Enter product name: ");
    scanf(" %[^\n]", product.name);

    printf("Enter product description: ");
    scanf(" %[^\n]", product.description);

    while (1) {
        printf("Enter product price (in dollars): ");
        if (scanf("%lf", &product.price) == 1) {
            break;
        }
        else {
            printf("Invalid price. Please enter a valid numeric value.\n");
            scanf("%*s");  // Consume the invalid input
        }
    }

    while (1) {
        printf("Enter product quantity: ");
        if (scanf("%d", &product.quantity) == 1) {
            break;
        }
        else {
            printf("Invalid input. Please enter a valid quantity.\n");
            scanf("%*s");  // Consume the invalid input
        }
    }

    while (getchar() != '\n');  // Consume the newline character left in the buffer

    printf("Enter supplier name: ");
    scanf(" %[^\n]", product.supplier);


    // Open the file for appending
    FILE* file = fopen("inventory.txt", "a");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    // Write the product data to the file, including remaining quantity
    fprintf(file, "Barcode: %s, Name: %s, Description: %s, Price: $%.2lf, Quantity: %d, Supplier: %s\n",
        product.barcode, product.name, product.description, product.price, product.quantity, product.supplier);

    // Close the file
    fclose(file);

    // Different arrays for sotring memory in case that a product already exists and does not exist
    if (productCount == 0) {
        products = (struct Product*)malloc(sizeof(struct Product));
    }
    else {
        products = (struct Product*)realloc(products, (productCount + 1) * sizeof(struct Product));
    }

    if (products == NULL) {
        printf("Memory allocation failed. Exiting...\n");
        exit(1);
    }

    // Increment the products quantity
    products[productCount++] = product;
    printf("Product added successfully.\n");
}


// Function to record a sale
void recordSale() {
    struct Sale sale;

    while (1) {
        printf("Enter product barcode for the sale (exactly 12 digits): ");
        scanf(" %[^\n]", sale.barcode);

        if (isValidBarcode(sale.barcode)) {
            break;
        }
        else {
            printf("Invalid barcode. Please enter a valid 12-digit barcode.\n");
        }
    }

    int productIndex = -1;
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].barcode, sale.barcode) == 0) {
            productIndex = i;
            break;
        }
    }

    if (productIndex == -1) {
        printf("Product with the provided barcode not found.\n");
        return;
    }

    printf("Enter quantity sold: ");
    char quantityStr[20];
    scanf(" %[^\n]", quantityStr);

    // Check if the entered quantity is a valid numeric value
    if (!isValidNumeric(quantityStr)) {
        printf("Invalid quantity. Please enter a valid numeric value.\n");
        return;
    }

    // Convert the valid numeric quantity to an integer
    sscanf(quantityStr, "%d", &sale.quantitySold);

    // Check if there is enough stock for the sale
    if (sale.quantitySold > products[productIndex].quantity) {
        printf("Not enough stock for the sale.\n");
        return;
    }

    while (1) {
        printf("Enter sale date (YYYY-MM-DD): ");
        scanf(" %[^\n]", sale.date);

        if (isValidDate(sale.date)) {
            break;
        }
        else {
            printf("Invalid date. Please enter a valid date.\n");
        }
    }

    printf("Enter customer name: ");
    scanf(" %[^\n]", sale.customer);

    // After recording a sale, check if the product quantity is less than 100
    if (products[productIndex].quantity - sale.quantitySold < 100) {
        printf("Alert: Product quantity is less than 100.\n");
    }

    // Open the file for appending
    FILE* file = fopen("sales.txt", "a");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    int remainingQuantity = products[productIndex].quantity - sale.quantitySold;
    // Write the sale data to the file
    fprintf(file, "Barcode: %s, Quantity Sold: %d, Remaining quantity: %d, Sale Date: %s, Customer: %s\n",
        sale.barcode, sale.quantitySold, remainingQuantity, sale.date, sale.customer);

    // Close the file
    fclose(file);

    if (saleCount == 0) {
        sales = (struct Sale*)malloc(sizeof(struct Sale));
    }
    else {
        sales = (struct Sale*)realloc(sales, (saleCount + 1) * sizeof(struct Sale));
    }

    if (sales == NULL) {
        printf("Memory allocation failed. Exiting...\n");
        exit(1);
    }

    sales[saleCount++] = sale;
    // Calculate the quantity of products after a sale
    products[productIndex].quantity -= sale.quantitySold;
    printf("Sale recorded successfully.\n");
}

// Function to display the product list
void displayProducts() {
    printf("Product Display:\n");
    for (int i = 0; i < productCount; i++) {
        printf("Barcode: %s, Name: %s, Description: %s, Price: $%.2lf, Quantity: %d, Supplier: %s\n",
            products[i].barcode, products[i].name, products[i].description,
            products[i].price, products[i].quantity - sumTotalSoldQuantity(products[i].barcode), products[i].supplier);
    }
}

// Function to calculate the total quantity sold for a product
int sumTotalSoldQuantity(const char* barcode) {
    int totalSoldQuantity = 0;
    for (int i = 0; i < saleCount; i++) {
        if (strcmp(sales[i].barcode, barcode) == 0) {
            totalSoldQuantity += sales[i].quantitySold;
        }
    }
    return totalSoldQuantity;
}


// Function to search for a product by barcode
void searchProductByBarcode() {
    char searchBarcode[13];  // Increase the size by 1 to accommodate null terminator

    while (1) {
        printf("Enter product barcode to search (exactly 12 digits): ");
        scanf(" %[^\n]", searchBarcode);

        if (isValidBarcode(searchBarcode)) {
            break;
        }
        else {
            printf("Invalid barcode. Please enter a valid 12-digit barcode.\n");
        }
    }

    int productIndex = -1;
    for (int i = 0; i < productCount; i++) {
        if (strcmp(products[i].barcode, searchBarcode) == 0) {
            productIndex = i;
            break;
        }
    }

    if (productIndex != -1) {
        printf("Product found:\n");
        printf("Barcode: %s, Name: %s, Description: %s, Price: $%.2lf, Quantity: %d, Supplier: %s\n",
            products[productIndex].barcode, products[productIndex].name, products[productIndex].description,
            products[productIndex].price, products[productIndex].quantity, products[productIndex].supplier);
    }
    else {
        printf("Product with the provided barcode not found.\n");
    }
}

// Function to free dynamically allocated memory
void cleanup() {
    free(products);
    free(sales);
}

int main() {
    int choice;

    while (1) {
        printf("\nInventory Management System\n");
        printf("1. Add Product\n");
        printf("2. Record Sale\n");
        printf("3. Display Products\n");
        printf("4. Search Product by Barcode\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            addProduct();
            break;
        case 2:
            recordSale();
            break;
        case 3:
            displayProducts();
            break;
        case 4:
            searchProductByBarcode();
            break;
        case 5:
            cleanup();
            exit(0);
        default:
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
