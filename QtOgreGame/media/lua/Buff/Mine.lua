Name="Mine";
Description="�������Ѿ�������һ�����ף��ڵо�ռ�������󴥷����۳��Է�1~3����������";
ImageName="Buff/Mine";

function Register()
	BuffManager:RegisterStateTrigger(Buff,"AfterFightWin");

end
function AfterFightWin()
	print("Mine Check");
	local area1=EventInfo:GetWinnerArea();
	local area2=EventInfo:GetAttackerArea();
	if(area1:GetID()==area2:GetID())then
		local MineBoom=math.random(0,3);
		local armycount=EventInfo:GetLoserArea():GetArmyCount()-MineBoom;
		EventInfo:GetLoserArea():SetArmyCount(armycount);
		print("Mine Boom,Lose "..MineBoom.." Army(s)");
	end
	
	return 1;
end