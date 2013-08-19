Name="Mine";
Description="此区域已经被安放一个地雷，在敌军占领此区域后触发。扣除对方1~3个部队数量";
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