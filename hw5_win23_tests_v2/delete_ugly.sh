directory="tests_regular"

# Iterate through all files in the directory
for file in "$directory"/*.in; do
    if [[ -f "$file" ]]; then
        # Replace 'x' with 'y' in each file
        sed -i 's/–/-/g' "$file"
    fi
done