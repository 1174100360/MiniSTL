#pragma once

#include <cstddef> // ptrdiff_t

//allocΪSGI STLĬ�Ͽռ�������
template<class T,class Alloc = alloc>
class vector {
public:
	//Ƕ�����ͱ�������
	using value_type = T;
	using pointer = value_type* ;
	using iterator = value_type* ;//vector������Ϊ��ָͨ��
	using reference = value_type& ;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

protected:
	//�ڴ����á��������캯��

	//��simple_allocΪ�ռ�������
	using data_allocator = simple_alloc<value_type, Alloc>;

	iterator start;//Ŀǰ��ʹ�õ�ͷ��
	iterator finish;//��ʹ��β��
	iterator end_of_stroage;//���ÿռ�β��

	void insert_aux(iterator position, const T& x);

	//�ͷű�vector��ռ�õ��ڴ�
	void deallocate() {
		if (start) {
			data_allocator::deallocate(start, end_of_stroage - start);
		}
	}

	//��ȡ����ʼ���ڴ�����
	void fill_initialize(size_type n, const T& value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_stroage = finish;
	}

	//���ÿռ䲢��������(����ʵ�ּ�allocator��
	iterator allocate_and_fill(size_type n, const value_type& value) {
		iterator result = data_allocator::allocate(n);
		uninitialized_fill_n(result, n, x);
		return result;
	}

public:
	//��������������
	vector() :start(nullptr), end(nullptr), end_of_stroage(nullptr) {}
	vector(size_type n, const value_type &value) { fill_initialize(n, value); }
	vector(long n, const value_type &value) { fill_initialize(n, value); }
	explicit vector(size_type n) { fill_initialize(n, value_type()); }

	~vector() {
		destory(start, finish);//ȫ�ֺ�������allocator
		deallocate();
	}

public:
	//��̬��д�ӿ�
	iterator begin() { return start; }
	iterator end() { return finish; }
	reference operator[](size_type n) { return *(begin() + n); }
	reference front() { return *begin(); }
	reference back() { return *(end() - 1); }

public:
	//��ֻ̬���ӿ�
	size_type size() const { return static_cast<size_type>(end() - begin()); }
	size_type capacity() const { return static_cast<size_type>(end_of_stroage - begin()); }
	bool empty() const { return begin() == end(); }

public:
	//��̬�ӿ�
	void push_back(const value_type&value) {
		if (finish != end_of_stroage) {
			construct(finish, x);//ȫ�ֺ���
			++finish;
		}
		else
			insert_aux(end(), value);
	}

	void pop_back() {
		--finish;
		destory(finish);
	}

	iterator erase(iterator position) {
		if (position + 1 != end()) {
			copy(position + 1, finish, position);
			pop_back(finish);
			return position;
		}
	}

	void resize(size_type new_size, const value_type& value) {
		if (new_size < size()) {
			erase(begin() + new_size, end());
		}
		else
			insert(end(),new_size-size(),value)
	}

	void resize(size_type new_size) {
		resize(new_size, value_type());
	}
};