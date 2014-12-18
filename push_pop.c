typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t limit;
  size_t size;
  int current;
  int *requests; // Pool for requestes file descriptors
} req_buffer_t;

void insert(req_buffer_t *b, req_t *req)
{
  // Increment pointer to current, and set
  pthread_mutex_lock(&b->lock);

  // Block until some requests are served
  while (b->current == (b->size - 1)) {
    printf("Blocked until some requets are served\n");
    pthread_cond_wait(&b->limit, &b->lock);
  }

  b->requests[++b->current] = fd;
  printf("Conn %d inserted on %d\n", fd, b->current);
  pthread_cond_signal(&b->limit);
  pthread_mutex_unlock(&b->lock);
}

int extract(req_buffer_t *b)
{
  pthread_mutex_lock(&b->lock);

  // Block until there is something in the pool
  while (b->current < 0) {
    printf("Block until there is something in the pool\n");
    pthread_cond_wait(&b->limit, &b->lock);
  }

  int fd = b->requests[b->current];

  printf("Conn %d poped from %d\n", fd, b->current);

  b->current--;

  pthread_cond_signal(&b->limit);
  pthread_mutex_unlock(&b->lock);

  return fd;
}
