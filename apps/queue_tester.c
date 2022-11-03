#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* queue_iterate() functions*/
static void plus_one(queue_t q, void* data)
{
	(void)q;
	int* a = (int*)data;

	(*a)++;
}
static void iterator_inc(queue_t q, void* data)
{
	int* a = (int*)data;

	if (*a == 42)
		queue_delete(q, data);
	else
		*a += 1;
}
static void delete_all(queue_t q, void* data)
{
	queue_delete(q, data);
}
static void dequeue_all(queue_t q, void* data)
{
	queue_dequeue(q, &data);
}

/* Test Cases */
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

	TEST_ASSERT(queue_destroy(q) == 0);			//test destroy

	printf("\n");
}

void test_queue_basic_unit(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_enqueue ***\n");

	q = queue_create();
	TEST_ASSERT(queue_enqueue(q, &data) == 0);		//test enqueue success
	TEST_ASSERT(queue_enqueue(NULL, &data) == -1);		//test enqueue NULL parameter
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);		//test enqueue NULL parameter

	printf("\n");

	fprintf(stderr, "*** TEST queue_dequeue ***\n");

	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == 0);	//test success
	TEST_ASSERT(*ptr == 3);					//test pointer assignment
	TEST_ASSERT(queue_dequeue(NULL, (void**)&ptr) == -1);	//test error
	TEST_ASSERT(queue_dequeue(q, NULL) == -1);		//test error
	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);	//test empty

	printf("\n");

	queue_destroy(q);
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

	TEST_ASSERT(queue_length(q) == 10);		//test enqueue success
	TEST_ASSERT(queue_delete(q, &data[5]) == 0);	//test MID-set deletion (42)
	TEST_ASSERT(queue_length(q) == 9);		//test deletion success
	TEST_ASSERT(queue_delete(q, &data[0]) == 0);	//test FIRST-in-set deletion
	TEST_ASSERT(queue_length(q) == 8);		//test deletion success
	TEST_ASSERT(queue_delete(q, &data[9]) == 0);	//test LAST-in-set deletion
	TEST_ASSERT(queue_length(q) == 7);		//test deletion success

	queue_iterate(q, dequeue_all);
	TEST_ASSERT(queue_destroy(q) == 0);		//test iteration dequeuing of ALL elements
	printf("\n");
}

void test_iterate()
{
	queue_t q;
	int data[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	size_t i;

	fprintf(stderr, "*** TEST queue_iterate ***\n");

	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	queue_iterate(q, plus_one);

	TEST_ASSERT(data[0] == 2);			//test iteration modification
	TEST_ASSERT(data[8] == 10);			//test iteration modification

	queue_iterate(q, dequeue_all);
	TEST_ASSERT(queue_destroy(q) == 0);		//test iteration dequeuing of ALL elements
	printf("\n");
}

void test_iterate_deletion()
{
	queue_t q;
	int data[] = { 1, 2, 3, 4, 5, 42, 6, 7, 8, 9 };
	size_t i;

	fprintf(stderr, "*** TEST queue_iterate_delete ***\n");

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	/* Increment every item of the queue, delete item '42' */
	queue_iterate(q, iterator_inc);
	TEST_ASSERT(data[0] == 2);			//test iteration modification
	TEST_ASSERT(queue_length(q) == 9);		//test iteration DELETION (42)

	queue_iterate(q, delete_all);

	TEST_ASSERT(queue_destroy(q) == 0);		//test iteration deletion of ALL elements
}

int main(void)
{
	test_create();
	test_destroy();
	test_queue_basic_unit();
	test_delete();
	test_iterate();
	test_iterate_deletion();

	return 0;
}
