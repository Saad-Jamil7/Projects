#include <iostream>
#include <cstring>
#include<fstream>
using namespace std;

const int INF = 9999;
const int ADDRESS = 13; 
const int RESTAURANT = 6;

int graph[ADDRESS][ADDRESS] = {0};
bool roadBlocked[ADDRESS][ADDRESS] = {false}; 

// Zone names mapping
string zoneNames[ADDRESS] = {
    "",              // 0 unused
    "Korangi",       // 1
    "Orangi",        // 2
    "Lasbella",      // 3
    "Paposh",        // 4
    "Nazimabad",     // 5
    "North Nazimabad",// 6 (Restaurant location)
    "Five Star",     // 7
    "Nagan",         // 8
    "Gulshan",       // 9
    "Johar",         // 10
    "Malir",         // 11
    "DHA"            // 12
};


struct Order {
    int zone;
    int id;
    string zoneName;
    char customerName[50];   
    char phoneNumber[10];     
    char items[100];
    float amount;
    int distance;
    int charge;
    Order* next;
};


Order* front = nullptr;
Order* rear = nullptr;
int orderCount = 0;


void setupRoutes() {
    
    
    // Main connections
    graph[1][2] = graph[2][1] = 4;   // Korangi - Orangi
    graph[2][4] = graph[4][2] = 6;   // Orangi - Paposh
    graph[3][5] = graph[5][3] = 2;   // Lasbella - Nazimabad
    graph[5][4] = graph[4][5] = 3;   // Nazimabad - Paposh
    graph[6][2] = graph[2][6] = 9;   // North Nazimabad - Orangi
    graph[4][6] = graph[6][4] = 2;   // North Nazimabad - Paposh
    graph[5][6] = graph[6][5] = 2;   // Nazimabad - North Nazimabad
    graph[6][7] = graph[7][6] = 2;   // North Nazimabad - Five Star
    graph[7][8] = graph[8][7] = 1;   // Five Star - Nagan
    graph[7][9] = graph[9][7] = 4;   // Five Star - Gulshan
    graph[8][9] = graph[9][8] = 4;   // Nagan - Gulshan
    graph[9][10] = graph[10][9] = 4; // Gulshan - Johar
    graph[9][11] = graph[11][9] = 10; // Gulshan - Malir
    graph[10][11] = graph[11][10] = 5; // Johar - Malir
    graph[11][12] = graph[12][11] = 9; // Malir - DHA
    
    roadBlocked[3][5] = roadBlocked[5][3] = true;   // Lasbella - Nazimabad
    roadBlocked[7][9] = roadBlocked[9][7] = true;  // Block Five Star - Gulshan
    roadBlocked[10][11] = roadBlocked[11][10] = true; // Block Johar - Malir
}

int dijkstra(int start, int end) {
    int dist[ADDRESS];
    int parent[ADDRESS];
    bool visited[ADDRESS] = {false};

    for (int i = 0; i < ADDRESS; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[start] = 0;

    for (int count = 0; count < ADDRESS - 1; count++) {
        int min = INF, u = -1;

        for (int v = 1; v < ADDRESS; v++)
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }

        if (u == -1) break;

        visited[u] = true;

        for (int v = 1; v < ADDRESS; v++)
            if (!visited[v] && graph[u][v] && !roadBlocked[u][v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
    }

    return dist[end];
}

void printShortestPath(int start, int end) {
    int dist[ADDRESS];
    int parent[ADDRESS];
    bool visited[ADDRESS] = {false};

    for (int i = 0; i < ADDRESS; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[start] = 0;

    for (int count = 0; count < ADDRESS - 1; count++) {
        int min = INF, u = -1;

        for (int v = 1; v < ADDRESS; v++)
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }

        if (u == -1) break;

        visited[u] = true;

        for (int v = 1; v < ADDRESS; v++)
            if (!visited[v] && graph[u][v] && !roadBlocked[u][v] && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
    }

    // Print the shortest path
    cout << "\nOptimal route: ";
    int path[ADDRESS];
    int pathLength = 0;
    int current = end;
    
    while (current != -1) {
        path[pathLength++] = current;
        current = parent[current];
    }
    
    for (int i = pathLength - 1; i >= 0; i--) {
        cout << zoneNames[path[i]];
        if (i > 0) cout << " -> ";
    }
    cout << " (Distance: " << dist[end] << " km)";
    
    // Check if route was affected by roadblocks
    bool routeBlocked = false;
    for (int i = pathLength - 1; i > 0; i--) {
        if (roadBlocked[path[i]][path[i-1]]) {
            routeBlocked = true;
            break;
        }
    }
    
    if (routeBlocked || dist[end] > dijkstra(start, end)) {
        cout << " *Alternative route due to roadblocks*";
    }
}

int deliveryCharge(int zone) {
    // Free delivery zones: Paposh,Nazimabad,Five Star
    if (zone == 4 || zone == 5 || zone == 7 )
        return 0;
    // Standard delivery:Sakhi Hasan,Lasbella,Orangi,Gulshan,Johar
    else if (zone == 2 || zone == 3 || zone==8 || zone == 9 || zone == 10)
        return 150;
    // High delivery due to distance:Korangi,Malir,DHA
    else
        return 200;
}

void saveOrderToFile(Order* order) {
    ofstream file("orders.txt", ios::app); // append mode
    if (file.is_open()) {
        file << "Order ID: " << order->id << "\n";
        file << "Name: " << order->customerName << "\n";
        file << "Phone: " << order->phoneNumber << "\n";
        file << "Delivery Area: " << order->zoneName << "\n";
        file << "Items: " << order->items << "\n";
        file << "Distance: " << order->distance << " km\n";
        file << "Delivery Charge: Rs. " << order->amount << "\n";
        file << "--------------------------\n";
        file.close();
    } else {
        cout << "Error opening file to save order.\n";
    }
}


// ------------------ Order System -------------------------

void enqueue(int zone, const char name[], const char phone[], const char item[], float rate, int distance, int charge) {
    Order* newOrder = new Order();
    newOrder->zone = zone;
    newOrder->zoneName = zoneNames[zone];
    strcpy(newOrder->customerName, name);
    strcpy(newOrder->phoneNumber, phone);
    strcpy(newOrder->items, item);
    newOrder->amount = rate;
    newOrder->distance = distance;
    newOrder->charge = charge;
    newOrder->next = nullptr;
newOrder->id = ++orderCount;
saveOrderToFile(newOrder);

    if (rear == nullptr) {
        front = rear = newOrder;
    } else {
        rear->next = newOrder;
        rear = newOrder;
    }
}


Order* dequeue() {
    if (front == nullptr) {
        return nullptr;
    }
    
    Order* temp = front;
    front = front->next;
    
    if (front == nullptr) {
        rear = nullptr;
    }
    
    return temp;
}

void displayOrders() {
    if (front == nullptr) {
        cout << "\nNo orders in the queue.\n";
        return;
    }

    Order* temp = front;
    int count = 1;
    cout << "\n=== Pending Orders ===";
    while (temp != nullptr) {
        cout << "\n" << string(50, '-');
        cout << "\nOrder #" << count++
             << "\nOrder Id: " << temp->id
             << "\nCustomer Name: " << temp->customerName
             << "\nPhone Number: " << temp->phoneNumber
             << "\nDelivery Area: " << temp->zoneName
             << "\nItems: " << temp->items
             << "\nFood Amount: Rs." << temp->amount
             << "\nDelivery Distance: " << temp->distance << " km"
             << "\nDelivery Charge: Rs." << temp->charge
             << "\nTotal Payable: Rs." << temp->amount + temp->charge;
        
        
        printShortestPath(RESTAURANT, temp->zone);
        cout << endl;
        temp = temp->next;
    }
    cout << string(50, '-') << endl;
}

void customerMenu() {
    int choice;
    do {
        cout << "\n=== Customer Menu ===";
        cout << "\n1. Place Order";
        cout << "\n2. Back to Main Menu";
        cout << "\nEnter your choice: ";
        cin >> choice;
        
        if (choice == 1) {
            
            // Customer Details
            char customerName[50];
            char phoneNumber[15];
            
            cout << "\n=== Customer Information ===";
            cout << "Enter your name: ";
            cin.ignore(); 
            cin.getline(customerName, 50);
            
            cout << "Enter your phone number: ";
            cin.getline(phoneNumber, 15);
            
            int zone;
            cout << "\n=== Available Delivery Areas ===";
            for (int i = 1; i <= 12; i++) {
                cout << "\n" << i << ". " << zoneNames[i];
            }
            
            cout << "\n\n=== Delivery Charges ===";
            cout << "\nFree Delivery: Paposh, Nazimabad, Five Star, Sakhi Hasan";
            cout << "\nRs.150 Delivery: Lasbella, Orangi, Gulshan, Johar";
            cout << "\nRs.200 Delivery: Korangi, Malir, DHA";
            cout << "\n\nSelect your area (1-12): ";
            cin >> zone;
            
            if (zone < 1 || zone > 12) {
                cout << "Invalid area selection!\n";
                continue;
            }
             int dist = dijkstra(RESTAURANT, zone);
             if (dist == INF) {
                    cout << "\nSorry " << customerName << ", due to current roadblocks,";
                    cout << "\nwe cannot reach " << zoneNames[zone] << " at this time.";
                    continue;
                }

            float total = 0;
            char finalItem[100] = "";
            int itemChoice;

            cout << "\nHello " << customerName << "!";
            cout << "\nDelivering to: " << zoneNames[zone];

            do {
                cout << "\nSelect items:";
                cout << "\n1. Pizza - Rs.750";
                cout << "\n2. Burger - Rs.550";
                cout << "\n3. Fries - Rs.100";
                cout << "\n4. Drinks - Rs.100";
                cout << "\nEnter item number (0 to finish): ";
                cin >> itemChoice;

                int qty;
                switch (itemChoice) {
                    case 1:
                        cout << "Enter quantity for Pizza: ";
                        cin >> qty;
                        total += 750 * qty;
                        strcat(finalItem, "Pizza(");
                        char qtyStr[10];
                        sprintf(qtyStr, "%d", qty);
                        strcat(finalItem, qtyStr);
                        strcat(finalItem, ") ");
                        break;
                    case 2:
                        cout << "Enter quantity for Burger: ";
                        cin >> qty;
                        total += 550 * qty;
                        strcat(finalItem, "Burger(");
                        char qtyStr2[10];
                        sprintf(qtyStr2, "%d", qty);
                        strcat(finalItem, qtyStr2);
                        strcat(finalItem, ") ");
                        break;
                    case 3:
                        cout << "Enter quantity for Fries: ";
                        cin >> qty;
                        total += 100 * qty;
                        strcat(finalItem, "Fries(");
                        char qtyStr3[10];
                        sprintf(qtyStr3, "%d", qty);
                        strcat(finalItem, qtyStr3);
                        strcat(finalItem, ") ");
                        break;
                    case 4:
                         cout << "Select Drink Type:\n";
                         cout << "1. Black (ColaNext/Coke)\n";
                         cout << "2. White (Fizzup/7Up)\n";
                         int drinkType;
                         cin >> drinkType;

                         cout << "Enter quantity: ";
                         cin >> qty;
                         total += 100 * qty;

                      if (drinkType == 1) {
                         strcat(finalItem, "B Drink(");
                         } else if (drinkType == 2) {
                            strcat(finalItem, "W Drink(");
                         } else {
                            strcat(finalItem, "Drink(");
                         }

                         char qtyStr4[10];
                         sprintf(qtyStr4, "%d", qty);
                         strcat(finalItem, qtyStr4);
                         strcat(finalItem, ") ");
                         break;

                    case 0:
                        break;
                    default:
                        cout << "Invalid item.\n";
                }
            } while (itemChoice != 0);

            if (strlen(finalItem) > 0) {
               
                
                int charge = deliveryCharge(zone);
                enqueue(zone, customerName, phoneNumber, finalItem, total, dist, charge);
                
                cout << "\n" << string(50, '=');
                cout << "\n=== Order Summary ===";
                cout << "\nOrder Placed Successfully!";
                cout << "\nOrder Id Number: " << orderCount;
                cout << "\nCustomer Name: " << customerName;
                cout << "\nPhone Number: " << phoneNumber;
                cout << "\nDelivery Area: " << zoneNames[zone];
                cout << "\nItems: " << finalItem;
                cout << "\nFood Amount: Rs." << total;
                cout << "\nDelivery Distance: " << dist << " km (from North Nazimabad)";
                cout << "\nDelivery Charge: Rs." << charge;
                cout << "\nTotal Bill: Rs." << total + charge;
                
                // Show the optimal route
                printShortestPath(RESTAURANT, zone);
                cout << "\n" << string(50, '=') << endl;
            } else {
                cout << "No items selected. Order not placed.\n";
            }
        }
        else if (choice == 2) {
            break;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 2);
}

void adminMenu() {
    int choice;
    do {
        cout << "\n=== Admin Menu ===";
        cout << "\n1. View Pending Orders";
        cout << "\n2. Dispatch Next Order";
        cout << "\n3. Back to Main Menu";
        cout << "\nEnter your choice: ";
        cin >> choice;
        
        if (choice == 1) {
            displayOrders();
        }
        else if (choice == 2) {
            Order* dispatchedOrder = dequeue();
            if (dispatchedOrder != nullptr) {
                cout << "\n" << string(50, '=');
                cout << "\n=== Order Dispatched ===";
                cout << "\nOrder Id"<<dispatchedOrder->id;
                cout << "\nCustomer Name: " << dispatchedOrder->customerName;
                cout << "\nPhone Number: " << dispatchedOrder->phoneNumber;
                cout << "\nDelivery Area: " << dispatchedOrder->zoneName;
                cout << "\nItems: " << dispatchedOrder->items;
                cout << "\nFood Amount: Rs." << dispatchedOrder->amount;
                cout << "\nDelivery Distance: " << dispatchedOrder->distance << " km";
                cout << "\nDelivery Charge: Rs." << dispatchedOrder->charge;
                cout << "\nTotal Amount: Rs." << dispatchedOrder->amount + dispatchedOrder->charge;
                cout << "\nStatus: Out for Delivery to " << dispatchedOrder->zoneName;
                
                // Show the route the delivery person should take
                printShortestPath(RESTAURANT, dispatchedOrder->zone);
                cout << "\n" << string(50, '=') << endl;
                
                // Free the memory
                delete dispatchedOrder;
                
                cout << "\nRemaining orders in queue:";
                displayOrders();
            } else {
                cout << "\nNo orders to dispatch!\n";
            }
        }
        
        else if (choice == 3) {
            break;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);
}

int main() {
    setupRoutes();
    int mainChoice;
    
    do {
        cout << "\n=== Online Delivery System with Smart Routing ===";
        cout << "\n1. Customer Login";
        cout << "\n2. Admin Login";
        cout << "\n3. Exit";
        cout << "\nEnter your choice: ";
        cin >> mainChoice;
        
        switch (mainChoice) {
            case 1:
                customerMenu();
                break;
            case 2:
                adminMenu();
                break;
            case 3:
                cout << "\nExiting system...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (mainChoice != 3);
    
   
    while (front != nullptr) {
        Order* temp = dequeue();
        delete temp;
    }
    
    return 0;
}

