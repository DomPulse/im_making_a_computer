def generate_arduino_array(m, n, i, j, k, l):
    # Create an m x n matrix filled with zeros initially
    array = [[0 for _ in range(n)] for _ in range(m)]
    
    # Calculate the area where the letter 'A' will be drawn
    for row in range(k, m - l):
        for col in range(i, n - j):
            # Draw 'A' pattern
            if (row == k or row == m - l - 1):  # Top and bottom horizontal lines
                array[row][col] = 1
            elif (row - k == col - i) or (row - k == n - j - col - 1):  # Diagonal lines
                array[row][col] = 1

    # Format the array as a string to be printed in Arduino syntax
    formatted_array = '{'
    for row in array:
        formatted_array += '{' + ', '.join(map(str, row)) + '},\n'
    formatted_array = formatted_array.rstrip(',\n') + '}'
    
    return formatted_array


# Define the size and padding for the array
div = 8
m, n = 800//div, 525//div  # m rows and n columns
i, j = (96+48)//div, 4//div    # Horizontal padding (left and right)
k, l = 35//div, 10//div    # Vertical padding (top and bottom)

# Generate and print the Arduino-friendly boolean array for the letter 'A'
print(generate_arduino_array(n, m, i, j, k, l))
print(m, n)
