#pragma once
#include "new.h"//��Ҫplacement new

template <class T1,class T2>
inline void construct(T1* p, T2 value) {
	new(p) T1(value);
}

template <class T>
inline void destroy(T* p) {
	p->~T();
}

//�跨����traits������������
template <class ForwardIterator>
inline void destory(ForwardIterator beg, ForwardIterator end) {
	_destory(beg, end, value_type(first);
}

//�ж�Ԫ���Ƿ���trivial destructor
template <class ForwardIterator,class T>
inline void _destory(ForwardIterator beg, ForwardIterator end, T*) {
	typedef typename _type_traits<T>::has_trivial_destructor trivial_destrcutor;
	_destory_aux(beg, end, trival_destructor());
}

//���Ԫ�ص�value_type������non��trivial destructor
template <class ForwardIterator>
inline void _destory_aux(ForwardIterator beg, ForwardIterator end, _false_type) {
	for (; beg != end; ++beg)
		destroy(&*beg);//�Ͼ����������������ĵ�ַ
}

//����trivial destructor
//�����������������޹�ʹ������ô������������һ��Ч���ϵľ޴��˷�
template <class ForwardIterator>
inline void _destroy_aux(ForwardIterator, ForwardIterator,_true_type) {}

//���char*��wchar_t*���ػ�
inline void destory(char*, char*) {}
inline void destory(wchar_t*, wchar_t*) {}