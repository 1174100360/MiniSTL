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
//Effective C++����enum���÷�
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
		union obj* free_list_link;//ָ����һ�ڵ�
		char client_data[1];//ָ����Դ
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
	static char* chunk_alloc(size_t size, int &nobjs);

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
char* __default_alloc_template<threads, inst>::start_free = nullptr;

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = nullptr;

template <bool threads, int inst>
__default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS] = { 
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};


//��free_list�޿�������ʱ���������ռ�
//�¿ռ�ȡ���ڴ�أ�Ĭ�ϻ�ȡ20���ڵ�(���飩
//���ڴ�ز��㣬���ȡ�Ľ�С��20
template<bool threads, int inst>
inline void* __default_alloc_template<threads, inst>::refill(size_t n){
	int nobjs = 20;
	//���Ե���chunk_alloc,ע��nobjs��pass-by-reference����
	char* chunk = chunk_alloc(n, nobjs);
	obj* volatile *my_free_list;
	obj* result;
	obj* current_obj, next_obj;

	//��ֻ��ȡ��һ��������ֱ�ӷ���������ߣ�������free_list
	if (1 == nobjs) return(chunk);
	my_free_list = free_list + FREELIST_INDEX(n);

	//��chunk�ռ��ڽ���free_list
	result = static_cast<obj*>(chunk);
	//����free_listָ���ڴ�ط���Ŀռ�
	//chunkָ����ڴ�ֱ�ӷָ��û���free_listָ��ʣ�£�19����٣�������
	*my_free_list = next_obj = static_cast<obj*>(chunk + n);
	for (int i = 1;i!=nobjs-1; ++i) {
		current_obj = next_obj;
		//��ǿתΪchar*����ΪchunkΪchar*
		next_obj = static_cast<obj*>(static_cast<char*>(next_obj) + n);
	    current_obj->free_list_link = next_obj;
	}
	next_obj->free_list_link = nullptr;
	return result;
}

//Ĭ��sizeΪ8��������
template<bool threads, int inst>
inline char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int & nobjs){
	char* result;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free;//�ڴ��ʣ��ռ�
	if (bytes_left > total_bytes) {
		//������������
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else if (bytes_left > size) {
		//������������һ����������
		nobjs = bytes_left / size;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else {
		//�ڴ��һ�����鶼�޷��ṩ
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);//��heap����������ڴ棬heap_size���������ô������Ӷ�����
		if (bytes_left > 0) {
			//��ǰ�ڴ�ػ���һ�����ڴ棬Ϊ�˲��˷ѷ����free_list
			obj* volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
			static_cast<obj*>(start_free)->free_list_link = my_free_list;
			*my_free_list = static_cast<obj*>(start_free);
		}

		//����heap�ռ��Բ����ڴ��
		start_free = static_cast<char*>(malloc(bytes_to_get));
		if (!start_free) {
			//heap�ռ䲻�㣬mallocʧ��
			obj* volatile *my_free_list, *p;
			//��free_list�м���Ƿ��з������������
			for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (!p) {
					//�������Է��������
					*my_free_list = p->free_list_link;//���뵱ǰ����
					start_free = static_cast<char*>(p);
					end_free = start_free + i;
					return(chunk_alloc(size, nobjs));//�ݹ����������nobjs����ʱ��Ȼ����else_if��֧
				}
			}
			end_free = nullptr;//�������Ҳ����ڴ�
			//���õ�һ���������۲����ܷ�����ڴ棬���׳��쳣
			start_free = static_cast<char*>(malloc_alloc::allocate(bytes_to_get));
		}
		heap_size += bytes_to_get;//��ռ�õĶ��ڴ�
		end_free += start_free + bytes_to_get;
		return chunk_alloc(size, nobjs);//��������������nobjs
	}
}

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
