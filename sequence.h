typedef struct sequence_s sequence;
struct sequence_s {
    int length;
    int speed;
    int *notes;
};

sequence* sequence_new(length, speed);
void sequence_free(sequence *seq);
