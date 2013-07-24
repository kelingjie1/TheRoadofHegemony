Name="Mine";
Description="此区域已经被安放一个地雷，在敌军占领此区域后触发。扣除对方1~3个部队数量";
ImageName="Buff/Mine";
function Register()
	BuffManager:RegisterStateTrigger(Buff,"AfterFightWin");

end
function AfterFightWin()
	print(Name.."-AfterFightWin");
	return 0;
end