#pragma once

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)//������һ��if�Ƿ����� �������Թ�
#include <iostream>
# define __THROW_BAD_ALLOC cerr << "out of memory" << endl;exit(1)
#endif

//һ��������
template<int inst>
class __malloc_alloc_template {
public:
	//����������new_handler��malloc_handler 
	//alias declaration
	using malloc_handler = void(*)();
private:
	//���º���ָ�����Դ����ڴ治������
	static void* oom_malloc(size_t);
	static void* oom_realloc(void*, size_t);
	static malloc_handler __malloc_alloc_oom_handler;	
public:
	static void* allocate(size_t n) {
		void* result = malloc(n);
		if (!result) result = oom_malloc(n);
		return result;
	}
	static void deallocate(void* p, size_t /*n*/) {
		free(p);
	}
	static void* reallocate(void* p, size_t /*old_sz*/,size_t new_sz) {
		void* result = realloc(p, new_sz);
		if (!result) oom_realloc(p, new_sz);
		return result;
	}

	//ģ��set_new_handler ԭ�����ڲ�δʹ��new��delete�����ڴ棬����޷�ʹ��set_new_handler
	static malloc_handler set_malloc_handler(malloc_handler f){
		malloc_handler old = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return old;
	}
};

//��ʼ��handler
template<int inst>
typename __malloc_alloc_template<inst>::malloc_handler __malloc_alloc_template<inst>::__malloc_alloc_oom_handler= nullptr;

template<int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t){
	malloc_handler my_alloc_handler;
	void* result;
	for (;;) {//���ϳ����ͷš�����
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (!my_malloc_handler) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler)();//����handler����ͼ�ͷ��ڴ�
		result = malloc(n);
		if (result) return result;
	}
}

template<int inst>
inline void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n){
	malloc_handler my_alloc_handler;
	void* result;
	for (;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (!my_malloc_handler) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler());
		result = realloc(p, n);
		if (result) return result;
	}
}

//ֱ�ӽ�instָ��Ϊ0
typedef __malloc_alloc_template<0> malloc_alloc;

//freelist�����趨
//�����ϵ��߽磬�������ޣ�freelist����
//�˴�ʹ��C++11 scoped enum��������ǿ���Ͷ��������Ľ�����ʹ��static_cast����˲���
enum  __freelist_setting:std::size_t
{__ALIGN=8, __MAX_BYTES = 128, __NFREELISTS = __MAX_BYTES / __ALIGN};



//�ڶ���������
template <bool threads,int inst>
class __default_alloc_template {
private:
	//��bytes�ϵ���8�ı���
	//tips:��x������y����������ȡ (x+y-1)&~(y-1)
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1)&~(__ALIGN - 1));
	}
private:
	//free_list�ڵ�
	//����union���ԣ�������Ҫռ�ö�����ڴ�
	union obj {
		union obj* free_list_link;
		char client_data[1];
	};
private:
	static obj* volatile free_list[__NFREELISTS];
	//����ʹ�õڼ��Žڵ㣬��1����
	static size_t FREELIST_INDEX(size_t bytes) {
		return (bytes + __ALIGN - 1) / __ALIGN - 1;
	}
	//����һ����СΪn�Ķ��󣬲��ҿ��ܼ����СΪn���������鵽free_list
	static void* refill(size_t n);
	//����һ���ռ䣬������nobjs����СΪsize������
	//����������� nobjs���ܻή��
	static char* chunk_alloc(size_t n, int &objs);

	//chunk allocation state
	static char *start_free;//�ڴ����ʼλ�ã�ֻ��chunk_alloc()�б仯
	static char *end_free;//�ڴ�ؽ���λ�ã�ֻ��chunk_alloc()�б仯
	static size_t heap_size;
public:
	static void* allocate(size_t n);
	static void deallocate(void *p, size_t n);
	static void* reallocate(void *p, size_t old_sz, size_t new_sz);
};

//����Ϊstatic data member�Ķ������ֵ�趨
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
__default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

template<bool threads, int inst>
inline void* __default_alloc_template<threads, inst>::allocate(size_t n){
	obj* volatile *my_free_list;
	obj* result;
	//��n����128,����õ�һ��������
	if (n >__MAX_BYTES)
		return(malloc_alloc::allocate(n));
	//ѡ����õڼ�����
	my_free_list = free_list + FREELIST_INDEX(n);
	result = *my_free_list;
	if (!result) {
		//δ�ҵ�����free_list��׼�����free_list
		void *r = refill(ROUND_UP(n));
		return r;
	}
	//����freelist
	*my_free_list = result->free_list_link;
	return(result);
}


template<bool threads, int inst>
inline void __default_alloc_template<threads, inst>::deallocate(void * p, size_t n){
	//p����Ϊnullptr
	obj* q = static_cast<obj*>(p);
	obj* volatile* my_free_list;
	//���е�һ��������
	if (n >__MAX_BYTES) {
		malloc_alloc::deallocate(p, n);
	}
	//Ѱ�Ҷ�Ӧ��free list
	my_free_list = free_list + FREELIST_INDEX(n);
	//����free list����������
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}
