Name="Mine";
Description="�������Ѿ�������һ�����ף��ڵо�ռ�������󴥷����۳��Է�1~3����������";
ImageName="Buff/Mine";

function Register()
	BuffManager:RegisterStateTrigger(Buff,"AfterFightWin");

end
function AfterFightWin()
	print("Mine Check");
	if(Buff:GetOwnerArea():GetID()==EventInfo:GetDefenderArea():GetID())then
		if(EventInfo:GetWinnerArea():GetID()==EventInfo:GetAttackerArea():GetID())then
			local MineBoom=math.random(0,3);
			local armycount=EventInfo:GetLoserArea():GetArmyCount()-MineBoom;
			EventInfo:GetLoserArea():SetArmyCount(armycount);
			GameStateManager:PlayAnimation("MineBoom");
			print("Mine Boom,Lose "..MineBoom.." Army(s)");
			return 1;
		end
	end
	return 0;
end