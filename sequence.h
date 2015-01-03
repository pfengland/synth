typedef struct sequence_s sequence;
struct sequence_s {
    int length;
    int speed;
    int *notes;
};

sequence* sequence_new(int length, int speed);
void sequence_free(sequence *seq);
