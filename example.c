#include <vec.h>

#ifdef LITE
#error "This example requires method pointers. Compile WITHOUT -DLITE."
#endif

typedef struct Person
{
    char name[16];
    int age;

} Person;

static void example_chars(void)
{
    Vec v = vec_new(sizeof(char));

    const char data[] = { 'a','b','c','d','e','f','g','h','i','j' };

    for (size_t i = 0; i < sizeof(data); ++i)
    {
        v.push(&v, &data[i]);
    }

    v.pop(&v); // remove 'j'
    v.reserve(&v, 64); // ensure capacity
    v.shrink(&v);      // shrink to len

    // remove 0-based index 2 ('c')
    v.remove(&v, 2);

    // print using method-style iterators, C-like syntax
    for (char *it = v.begin(&v); it != v.end(&v); ++it)
    {
        printf("%c ", *it);
    }
    printf("| at[1]=%c | back=%c\n",
           *(char*)v.at(&v, 1),
           v.back(&v) ? *(char*)v.back(&v) : '?');

    v.clear(&v);

    v.destroy(&v);

    return;
}

static void example_ints(void)
{
    Vec v = vec_new(sizeof(int));

    // reserve then push a few ints
    v.reserve(&v, 8);

    for (int i = 1; i <= 5; ++i) v.push(&v, &i);

    // remove middle element
    v.remove(&v, 2);

    // show contents using method-style iterators
    for (int *it = v.begin(&v); it != v.end(&v); ++it)
    {
        printf("%d ", *it);
    }
    printf("| at[0]=%d | back=%d\n",
           *(int*)v.at(&v, 0),
           v.back(&v) ? *(int*)v.back(&v) : -1);

    // pop and shrink
    v.pop(&v);
    v.shrink(&v);

    v.destroy(&v);

    return;
}

static void example_structs(void)
{
    Vec people = vec_new(sizeof(Person));

    Person a = { "Ada", 36 };
    Person b = { "Bob", 28 };
    Person c = { "Cyd", 41 };

    people.push(&people, &a);
    people.push(&people, &b);
    people.push(&people, &c);

    // remove Bob (index 1)
    people.remove(&people, 1);

    // print remaining using method-style iterators
    for (Person *it = people.begin(&people); it != people.end(&people); ++it)
    {
        printf("%s(%d) ", it->name, it->age);
    }

    printf("| at[0]=%s | back=%s\n",
           ((Person*)people.at(&people, 0))->name,
           people.back(&people) ? ((Person*)people.back(&people))->name : "<none>");

    people.clear(&people);

    people.destroy(&people);

    return;
}

int main(void) {
    example_chars();
    example_ints();
    example_structs();
    return 0;
}
