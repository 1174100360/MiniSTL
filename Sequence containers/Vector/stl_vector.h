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

	//��simple_allocΪ�ռ�������,������Ԫ�ش�С�������ÿռ�
	using data_allocator = simple_alloc<value_type, Alloc>;

	iterator start;
	iterator finish;
	iterator end_of_stroage;//���ÿռ�β��

	//����ӿڣ��ں����ݲ���
	void insert_aux(iterator position, const value_type& value);

	//�ͷű�vector��ռ�õ��ڴ�
	void deallocate() {
		if (start) {
			data_allocator::deallocate(start, end_of_stroage - start);
		}
	}

	//��ȡ����ʼ���ڴ�����
	void fill_initialize(size_type n, const value_type& value) {
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

	iterator erase(iterator first, iterator last) {
		iterator i = copy(last, finish, first);
		destory(i, finish);//��ʱi���ȼ���new_finish
		finish -= (last - first);
		return first;
	}

	iterator erase(iterator position) {
		if (position + 1 != end()) //��ȴβ�˽ڵ�����踴��
			copy(position + 1, finish, position);
		pop_back();
		return position;
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

	void insert(iterator position, size_type n, const value_type& value);
};

template<class T, class Alloc>
inline void vector<T, Alloc>::insert_aux(iterator position, const value_type& value){
	if (finish != end_of_stroage) {
		//��ǰ���ڱ��ÿռ�
		construct(finish, *(finish - 1));//�����һ��Ԫ��Ϊ��ֵ����Ԫ����finish
		++finish;
		value_type value_copy = value;//STL copy in copy out
		copy_backward(position, finish - 2, finish - 1);//��[pos,finish-2)copy��finish-1����finish-1ΪĿ���յ㣩
		*position = value_copy;
	}
	else {
		//����
		const size_type old_size = size();
		const size_type new_size = old_size ? 2 * old_size : 1;//2����С
		iterator new_start = data_allocator::allocate(new_size);
		iterator new_finish = newstart;
		try{
			new_finish = unitialized_copy(start,position,new_start);//����ǰ���
			construct(new_finish, value);
			++new_finish;
			new_finish = unitialized_copy(position, finish, new_finish);//���ƺ���
		}
		catch{
			//commit or rollback
			destory(new_start, new_finish);
			data_allocator::deallocate(new_start, new_size);
			throw;
		}
		//�ͷ�ԭ��vector
		destory(begin(), end());
		deallocate();
		//����������ָ����vector
		start = new_start;
		finish = new_finish;
		end_of_stroage = new_start + new_size;
	}
}

template<class T, class Alloc>
inline void vector<T, Alloc>::insert(iterator position, size_type n, const value_type & value){
	if (n) {
		if (static_cast<size_type>(end_of_stroage - finish) >= n) {
			//���ÿռ����
			value_type value_copy = value;
			const size_type elems_after = finish - position;
			iterator old_finish = finish;
			if (elems_after > n) {
				//������Ԫ�ظ���m>=����Ԫ�ظ���n
				unitialized_copy(finish - n, finish, finish);//�ȸ��ƺ�n��Ԫ��
				finish += n;
				copy_backward(position, old_finish - n, old_finish);//����m-n��Ԫ��
				fill(position, position + n, value_copy);
			}
			else {
				unitialized_fill_n(finish, n - eles_after, value_copy);//��m-n��value���ĩβ
				finish += n - elems_after;
				unitialized_copy(position, old_finish, finish);//��m���������ĩβ
				finish += elems_after;
				fill(position, old_finish,value_copy);//����m
			}
		}
		else {
			//��Ҫ����
			const size_type old_size = size();
			const size_type new_size = oldsize + max(oldsize, n);
			iterator new_start = data_allocator::allocate(new_size);
			iterator new_finish = newstart;
			__STL_TRY{
				new_finish = unitialized_copy(start,position,new_start);
				new_finish = unitialized_fill_n(new_finish, n, value);
				new_finish = unitialized_copy(position, finish, new_finish);
			}
#ifdef __STL_USE_EXCEPTIONS
			catch{
				destory(new_start, new_finish);
				data_allocator::deallocate(new_start, new_size);
				throw;
			}
#endif /* __STL_USE_EXCEPTIONS*/
			destory(begin(), end());
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_stroage = new_start + new_size;
		}
	}
}
