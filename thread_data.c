#include "metroid.h"

void	server_data_set(t_ring_buffer *buffer, t_message *message)
{
	SDL_LockMutex(buffer->mutex);
	while ((buffer->head + 1) % RING_BUFFER_SIZE == buffer->tail)
		SDL_CondWait(buffer->not_full, buffer->mutex);
	buffer->buffer[buffer->head++] = *message;
	buffer->head %= RING_BUFFER_SIZE;
	SDL_CondSignal(buffer->not_empty);
	SDL_UnlockMutex(buffer->mutex);
}

t_message	server_data_get(t_ring_buffer *buffer)
{
	t_message message;

	SDL_LockMutex(buffer->mutex);
	while (buffer->head == buffer->tail)
		SDL_CondWait(buffer->not_empty, buffer->mutex);
	message = buffer->buffer[buffer->tail++];
	buffer->tail %= RING_BUFFER_SIZE;
	SDL_CondSignal(buffer->not_full);
	SDL_UnlockMutex(buffer->mutex);

	return (message);
}