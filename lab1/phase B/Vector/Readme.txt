Ver2.0 将Ver1.0中push_back() push_front()中的resize函数去掉，将所有操作直接写在一个函数里。

	这是因为考虑到可能会有x.push_back(x[0])的情况。若用resize函数，会先将原来的x[0]空间destory掉。

	故改动主要在于改变了destory()和placement new的操作顺序



值得一提的是，Ver1.0 和 Ver2.0 都能通过老师的testcase。但是自己感觉Ver1.0是错了的。


最后交了Ver2.0