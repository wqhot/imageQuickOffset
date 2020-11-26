offset_x = importdata('boundlessJDI_x.csv');
offset_y = importdata('boundlessJDI_y.csv');
ori_x = repmat(0:1:1919,720,1);
ori_y = repmat([0:1:719]',1,1920);
final_x = (offset_x + ori_x);
final_y = (offset_y + ori_y);
final_xy = [final_x(:),final_y(:)];
u_final_xy=unique(final_xy);
% figure(1);
scatter(round(final_xy(:,1)), round(final_xy(:,2)),'.');
grid on;
axis equal;
hold on;
scatter((final_xy(:,1)), (final_xy(:,2)),'*');
% plot(final_xy,'.');
% hold on;
% for i=1:720
%     for j=1:1920
%         plot(final_x(i, j)+1,final_y(i, j)+1,'.');
%     end
% end
% plot(final_x(i, j)+1,final_y(i, j)+1,'.');