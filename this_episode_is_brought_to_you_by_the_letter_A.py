import random

def generate_boolean_row(n, m, w):
    if n + m > w:
        raise ValueError("Leading and trailing zeros exceed total length.")
    
    # Generate the middle section with random 0s and 1s
    middle_section_length = w - (n + m)
    middle_section = [random.randint(0, 1) for _ in range(middle_section_length)]

    # Construct the full row
    row = [0] * n + middle_section + [0] * m

    # Format for Arduino-style output
    formatted_row = "{" + ", ".join(map(str, row)) + "};"
    return formatted_row

# Example usage
n = 100  # Leading zeros
m = 100 # Trailing zeros
w = 300 # Total row length

print(generate_boolean_row(n, m, w))
