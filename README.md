# Bus Schedule App

This is a travel search and booking application designed to assist users in finding and booking trips. The application includes the following actors:

### Operator
- **Login Activities**: Operators can log in using their existing accounts. 
- **Add/Remove Trips**: Operators have the ability to add new coach trips to the system or remove existing ones.

### User
- **Account Creation**: Users can create new accounts to access the app's features.
- **Login**: Existing users can log in to their accounts.
- **Race Search**: Users can search for available coach trips based on their preferences.
- **Booking Activities**: Users can book coach trips that match their criteria.

## Exception Handling

The application implements exception handling methods to handle various scenarios:

### Operator
- **Wrong Login**: If an operator enters incorrect login credentials (wrong username or password).
- **Add Wrong Trip Details**: If an operator attempts to add a coach trip with incorrect details (e.g., wrong date format, past date, city with non-existent characters).

### User
- **Unsuccessful Login**: If a user's login attempt fails due to incorrect credentials.
- **Account Creation Errors**: If errors occur during account creation, such as invalid email format, weak password, or incorrect password confirmation.
- **No Trip**: If no coach trips are found matching the user's search criteria.
- **Wrong Coach Booking Details Entered**: If a user enters incorrect details while booking a coach trip.

## Files

- **app.cpp**: Contains the main program logic.
- **bus_schedule.csv**: Stores information about coach trips.
- **operators.csv**: Stores operator account details.
- **users.csv**: Stores user account details.

Feel free to explore the source code and CSV files to understand the functionality and data structure of the Bus Schedule App.
