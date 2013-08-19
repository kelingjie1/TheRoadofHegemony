Name="Mine";
Description="在所选区域设置一个地雷，在敌军占领此区域后触发。扣除对方1~3个部队数量";
ImageName="Card/Mine";
function Use()
	choose1=GameStateManager:GetChooseID(1);
	choose2=GameStateManager:GetChooseID(2);
	if(choose1~=-1 and choose2==-1) then
		area=Terrain:GetArea(choose1);
		if(area:GetAreaBelongID()==GameStateManager:GetCurrentPlayer():GetID()) then
			buff=BuffManager:CreateNewBuff("Mine");
			area:AddBuff(buff);
			re=1;
			print(Name.."-Use");
		else
			print(Name.."-区域不属于该玩家");
		end
	else
		print(Name.."-没有选择区域");
		re=0;
	end
	return re;
end