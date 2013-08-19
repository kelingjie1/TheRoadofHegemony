n=terrain:GetAreaCount();
--math.randomseed(os.time());
math.randomseed(0);
for i=0,n-1 do
	area=terrain:GetArea(i);
	if area:GetAreaBelongID()==0 then
		x=math.random(0,8);
		area:SetArmyCount(x);
	else
		area:SetArmyCount(6);
	end
	
end