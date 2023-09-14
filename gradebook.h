#define MAX_NAME_LEN 64
#define NUM_BUCKETS 1741

// Data type for elements in each hash bucket
typedef struct node {
    char name[MAX_NAME_LEN]; // Student's Name
    int score;               // Student's Assignment Score
    struct node *next;       // Next node in list, or NULL if no next node
} node_t;

// Gradebook data type
typedef struct {
    char class_name[MAX_NAME_LEN]; // Name of class for grades
    node_t *buckets[NUM_BUCKETS];  // Hash table buckets (linked list heads)
    unsigned size;                 // Total number of entries in gradebook
} gradebook_t;


// Create a new gradebook instance
// class_name: The name of the class for grades
// Returns: Pointer to a gradebook_t representing an empty gradebook
//          or NULL if an error occurs
gradebook_t *create_gradebook(const char *class_name);

// Returns a pointer to the gradebook class name
// book: A pointer to a gradebook to get the class name of
// Returns: Pointer to gradebook's class name (not to be modified)
const char *get_gradebook_name(const gradebook_t *book);

// Add a new score to the gradebook (insert into hash table)
// book: A pointer to a gradebook to add the score to
// name: Student's name
// score: Student's score
// Returns: 0 if the score was successfully added/updated
//          or -1 if the score could not be added/updated
int add_score(gradebook_t *book, const char *name, int score);

// Search for a specific student's score in the gradebook
// book: A pointer to a gradebook to search for the student score in
// name: The student's name
// Returns: The student's score if their name is found
//          or -1 if no matching student name is found
int find_score(const gradebook_t *book, const char *name);

// Print out all scores in the gradebook
// book: A pointer to the gradebook containing the scores to print
void print_gradebook(const gradebook_t *book);

// Frees all memory used to store the contents of the gradebook
// book: A pointer to the gradebook to free
void free_gradebook(gradebook_t *book);

// Write out all scores in the gradebook to a text file
// book: A pointer to the gradebook containing the scores to write out
int write_gradebook_to_text(const gradebook_t *book);

// Read in all scores from a text file and add to a new gradebook
// file_name: The name of the text file to read
// Returns: A pointer to a new gradebook with all scores as recorded in the file
//          or NULL if the read operation fails
gradebook_t *read_gradebook_from_text(const char *file_name);

// Write all scores in a gradebook to a binary file
// book: A pointer to a gradebook containing the scores to write out
int write_gradebook_to_binary(const gradebook_t *book);

// Read in all scores from a binary file and add to a new gradebook
// file_name: The name of the binary file to read
// Returns: A pointer to a new gradebook containing all scores as recorded in the file
//          or NULL if the read operation fails
gradebook_t *read_gradebook_from_binary(const char *file_name);
