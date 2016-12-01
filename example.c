/*
* An example of using linux kernel radix tree.
* Refer to [https://lwn.net/Articles/175432/] for more information.
* Author: Hongwei Qin
* Huazhong University of Science and Technology
* December, 2016
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/radix-tree.h>

struct mystr{
	unsigned long n;
};

struct mystr array[20];

//RADIX_TREE(mytree, GFP_ATOMIC);
struct radix_tree_root mytree;

void initTree(void)
{
	unsigned long i;
	int ret;
	/*
	It should be [&mytree] instead of [mytree]. The document is wrong.
	*/
	INIT_RADIX_TREE(&mytree, GFP_ATOMIC);
	for(i=0;i<20;i++){
		array[i].n = i*10;
		ret = radix_tree_insert(&mytree, i,&array[i]);
		if(ret)
		{
			printk(KERN_ERR"insert failed: %lu\n",i);
		}
	}
	return;
}

/*
* radix_tree_delete() removes the item associated with key from tree, 
* returning a pointer to that item if it was present.
*/
void deleteTree(void)
{
	unsigned long i;
	struct mystr *ptr;
	for(i=0;i<20;i++)
	{
		ptr = radix_tree_delete(&mytree, i);
		if(ptr)
			printk("delete item %lu, which value is %lu\n",i,ptr->n);
	}
	return;
}

/*
* The simplest form, radix_tree_lookup(), 
* looks for key in the tree and returns the associated item (or NULL on failure).
*/
void lookupway1(void)
{
	unsigned long lucky = 5;
	struct mystr *ptr = radix_tree_lookup(&mytree, lucky);
	if(!ptr){
		printk(KERN_ERR"lookupway1 failed!\n");
		return;
	}
	printk(KERN_NOTICE"lookupway1: value=%lu\n",ptr->n);
	return;
}

/*
* radix_tree_lookup_slot() will, instead, return a pointer to the slot holding the pointer to the item. 
* The caller can, then, change the pointer to associate a new item with the key. 
* If the item does not exist, however, radix_tree_lookup_slot() will not create a slot for it, 
* so this function cannot be used in place of radix_tree_insert().
*/
void lookupway2(void)
{
	unsigned long lucky = 4;
	unsigned long lucky2 = 2;
 	struct mystr **pptr = (struct mystr **)radix_tree_lookup_slot(&mytree, lucky);
	if(!pptr){
		printk(KERN_ERR"lookupway2 failed!\n");
		return;
	}
	printk(KERN_NOTICE"lookupway2: value=%lu\n",(*pptr)->n);
	printk(KERN_NOTICE"change the value to array[%lu]\n",lucky2);
	*pptr = &array[lucky2];
	return;
}

/*
* Finally, a call to radix_tree_gang_lookup() will return up to max_items items in results, 
* with ascending key values starting at first_index. The number of items returned may be less 
* than requested, but a short return (other than zero) does not imply that there are no more values in the tree.
*/
void lookupway3(void)
{
	unsigned long lucky1 = 2, lucky2 = 5;//5 items start from 2
	unsigned int ret;
	int i;
	void *results[10];

	ret = radix_tree_gang_lookup(&mytree, results,	lucky1, lucky2);
	printk(KERN_NOTICE"lookupway2: %lu items start from %lu, ret=%u\n",lucky2,lucky1,ret);
	for(i=0;i<lucky2;i++)
	{
		printk(KERN_NOTICE"item %d is %lu\n",i,((struct mystr *)results[i])->n);
	}
	return;
}



static int __init re_init(void)
{
	printk(KERN_NOTICE"init radix tree example\n");
	initTree();
	lookupway1();
	lookupway2();
	lookupway3();
	
	return 0;
}


static void __exit re_exit(void)
{
	printk(KERN_NOTICE"exit radix tree example\n");
	deleteTree();
	return;
}


MODULE_AUTHOR("Hongwei Qin");
MODULE_DESCRIPTION("an example of using linux kernel radix tree");
MODULE_LICENSE("GPL");
module_init(re_init);
module_exit(re_exit);

