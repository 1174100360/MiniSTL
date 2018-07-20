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
private:
	//���º���ָ�����Դ����ڴ治������
	static void* oom_malloc(size_t);
	static void* oom_realloc(void*, size_t);
	static void* (*__malloc_alloc_oom_handler)();
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
	//set_malloc_handler��һ������ָ�룬�䷵��һ������ָ�� void*����
	//���β���һ���޷���ֵ�ĺ���ָ�룬�����β�
	static void (*set_malloc_handler(void (*f)()))(){
		void (*old)() = __malloc_alloc_oom_handler;
		__malloc_alloc_oom_handler = f;
		return old;
	}
};

//��ʼhandler
template<int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = nullptr;

template<int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t)
{
	void(*my_malloc_handler)();
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
inline void * __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n){
	void(*my_malloc_handler)();
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



enum{__ALIGN=8};//С��������ϵ��߽�
enum{__MAX_BYTES=128};//��������
enum{__NFREELISTS=__MAX_BYTES/__ALIGN};//����ڵ���

//�ڶ���������
template <bool threads,int inst>
class __default_alloc_template {
private:
	//��bytes�ϵ���8�ı���
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

};