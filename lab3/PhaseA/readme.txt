����Ҫ���ľ�����LifeForm.cpp�����LifeForm.h�ﶨ��ģ���û����LifeForm-Craig.cpp��ʵ�ֵĺ���ʵ�ֳ���


	���е�LifeForm�ڱ���object�����Լ���λ�ӣ�pos����ͬʱ��ÿ��LifeForm��QuadTree�������Ӧ��λ�á�������һ�����Ҫ��������λ�õ�ͬ����

	��QuadTree�ÿ��LifeForm�Ķ��������region_resize������ʵ�����ǽ�region_resizeͨ��Lammda���ʽ��һ��object����ʽ��������һ����š����region_resize��������һ��callback����ν��callback���������ҵ����������ִ�еģ���Event�ﲻһ����ÿ����Ҫ�ı�QuadTree�����λ��ʱ����Ҫ�Ķ����Ǹ�LifeForm�ͻ������������callback����region_resize

	LifeForm�����update_position�����QuadTree�����update_position��QuadTree�����update_position������callback����region_resize��region_resize�ֻ����LifeForm��update_position...

	ʣ�µ�encounter����˵���ͱȽ������

	
������һ����������perceive���������˵Ӧ������Ҫ��update_position��perceive�����������update�����simulator�п��ܻ�crash��������update��perceive�򲻻�crash

lifeform�ļ������ǳ���һ��ʼ��algea��Craig����������lifeform�������������simulator