#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("\tPASS\n");				\
	} else	{							\
		printf("\tFAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
	printf("\n");
}

void test_destroy(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST destroy ***\n");

	q = queue_create();

	TEST_ASSERT(queue_destroy(q) == 0);

	printf("\n");
}

/* Enqueue/Dequeue simple */
void test_queue_basic_unit(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_enqueue ***\n");

	q = queue_create();
	TEST_ASSERT(queue_enqueue(q, &data) == 0);
	TEST_ASSERT(queue_enqueue(NULL, &data) == -1);
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);

	printf("\n");

	fprintf(stderr, "*** TEST queue_dequeue ***\n");

	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);
	TEST_ASSERT(queue_dequeue(NULL, (void**)&ptr) == -1);
	TEST_ASSERT(queue_dequeue(q, NULL) == -1);
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);	//test empty

	printf("\n");
}

void test_delete()
{
	queue_t q;
	int data[] = { 1, 2, 3, 4, 5, 42, 6, 7, 8, 9 };
	size_t i;

	fprintf(stderr, "*** TEST queue_delete ***\n");

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	TEST_ASSERT(queue_delete(q, &data[5]) == 0);
	TEST_ASSERT(queue_delete(NULL, &data[5]) == -1);
	TEST_ASSERT(queue_delete(q, NULL) == -1);

	int trash = 6;
	TEST_ASSERT(queue_delete(q, &trash) == -1);

	printf("\n");
}

void test_iterate()
{

}

int main(void)
{
	test_create();
	test_destroy();
	test_queue_basic_unit();
	test_delete();

	return 0;
}
