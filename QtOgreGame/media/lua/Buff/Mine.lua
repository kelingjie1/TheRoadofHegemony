Name="Mine";
Description="�������Ѿ�������һ�����ף��ڵо�ռ�������󴥷����۳��Է�1~3����������";
ImageName="Buff/Mine";
function Register()
	BuffManager:RegisterStateTrigger(Buff,"AfterFightWin");

end
function AfterFightWin()
	print(Name.."-AfterFightWin");
	return 0;
end