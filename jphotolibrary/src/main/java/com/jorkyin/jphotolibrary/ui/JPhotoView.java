package com.jorkyin.jphotolibrary.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.jorkyin.jphotolibrary.JPhotoViewItemDecoration;
import com.jorkyin.jphotolibrary.R;
import com.jorkyin.jphotolibrary.base.JPhoto;
import com.jorkyin.jphotolibrary.presenter.JPhotoViewPresenter;

import java.util.ArrayList;
import java.util.List;

public class JPhotoView extends RecyclerView {
    private Context mContext;
    private PhotoAdapter mAdapter;
    private int mSpanCount = 3;
    private ArrayList<JPhoto> mRTPhotos = new ArrayList<>();
    private GridLayoutManager mGLM;
    private onClickItemListener mOnClickItemViewListener;

    private JPhotoViewPresenter presenter;

    public JPhotoView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        this.mContext = context;
        onCreate();
    }

    private void onCreate() {
        mGLM = new GridLayoutManager(mContext, mSpanCount);
        mGLM.setSpanSizeLookup(new GridLayoutManager.SpanSizeLookup() {//最后一行组里面的最后一个占领多少个
            @Override
            public int getSpanSize(int position) {
                if (position + 1 == mRTPhotos.size()) {//list最后的数据,不用处理  直接返回1
                    return 1;

                }
                if (mRTPhotos.get(position).getTitleName().equals(mRTPhotos.get(position + 1).getTitleName())) {//判断是不是每一组的最后一个 ture：不是
                    return 1;
                } else {
                    int x = 1;
                    for (int i = position; i > 0; i--) {
                        if (mRTPhotos.get(position).getTitleName().equals(mRTPhotos.get(i - 1).getTitleName())) {//通过循环判断是在当前组的第几个
                            x++;
                        } else {
                            break;
                        }
                    }
                    if (x % mSpanCount == 0) {//最后一组刚好排满
                        return 1;
                    } else {
                        return mSpanCount - x % mSpanCount + 1;
                    }
                }
            }
        });
        setLayoutManager(mGLM);
        mAdapter = new PhotoAdapter();
        JPhotoViewItemDecoration RTPhotoItemDecoration = new JPhotoViewItemDecoration(mContext, mSpanCount, new JPhotoViewItemDecoration.DecorationCallback() {
            //获取同组中的第一个内容
            @Override
            public int getGroupId(int position) {
                int x = 1;
                for (int i = position; i > 0; i--) {
                    if (mRTPhotos.get(position).getTitleName().equals(mRTPhotos.get(i - 1).getTitleName())) {//通过循环判断是在当前组的第几个
                        x++;
                    } else {
                        break;
                    }
                }
                return x;
            }

            @Override
            public String getGroupString(int position) {
                return mRTPhotos.get(position).getTitleName();
            }
        });
        addItemDecoration(RTPhotoItemDecoration);
        setAdapter(mAdapter);
    }

    public void addPhoto(JPhoto JPhoto) {
        mRTPhotos.add(mRTPhotos.size(), JPhoto);
        mAdapter.notifyItemInserted(mRTPhotos.size() - 1);
    }

    public void addPhotos(ArrayList<JPhoto> RTPhotos) {
        mRTPhotos.addAll(RTPhotos);
        mAdapter.notifyDataSetChanged();
    }

    public void clearPhotos() {
        mRTPhotos.clear();
        removeAllViews();
    }

    public void cloceSelectVisibilityPhoto() {
        for (int i = 0; i < mRTPhotos.size(); i++) {
            mRTPhotos.get(i).setVisibility(false);
            mRTPhotos.get(i).setSelect(false);
        }
        // mAdapter.notifyItemChanged("payloads");
        mAdapter.notifyDataSetChanged();
    }

    class PhotoAdapter extends Adapter<PhotoAdapter.ViewHolder> {
        private static final String TAG = "PhotoAdapter";

        PhotoAdapter() {
        }

        @NonNull
        @Override
        public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            // 实例化展示的view
            View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_galler_photo, parent, false);
            // 实例化viewholder

            return new ViewHolder(view);
        }

        @Override
        public void onBindViewHolder(@NonNull ViewHolder holder, final int position, @NonNull List<Object> payloads) {
            super.onBindViewHolder(holder, position, payloads);
           /* if (payloads.isEmpty()) {
                Log.i(TAG, "onBindViewHolder:01 position=" + position);
                holder.item_iv_src.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_LOCAL) {
                            if (mRTPhotos.get(position).isVisibility()) {
                                mRTPhotos.get(position).setSelect(!mRTPhotos.get(position).isSelect());
                                notifyItemChanged(position, "payloads");
                            } else {
                                mOnClickItemViewListener.onRecyclerViewItemClick(view, mRTPhotos, position);
                            }
                        } else if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_SD) {
                            mOnClickItemViewListener.onRecyclerViewItemClick(view, mRTPhotos, position);
                        }
                    }
                });
                holder.item_iv_src.setOnLongClickListener(new View.OnLongClickListener() {
                    @Override
                    public boolean onLongClick(View v) {
                        if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_LOCAL) {
                            mRTPhotos.get(position).setSelect(!mRTPhotos.get(position).isSelect());
                            for (int i = 0; i < mRTPhotos.size(); i++) {
                                mRTPhotos.get(i).setVisibility(true);
                                notifyItemChanged(i, "payloads");
                            }
                            mOnClickItemViewListener.onRecyclerViewItemOnLongClick(v, position);
                        }
                        return true;
                    }
                });
                holder.item_iv_select.setOnClickListener(new View.OnClickListener() {//选定获取取消
                    @Override
                    public void onClick(View view) {
                        if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_LOCAL) {
                            if (mRTPhotos.get(position).isVisibility()) {
                                mRTPhotos.get(position).setSelect(!mRTPhotos.get(position).isSelect());
                                notifyItemChanged(position, "payloads");
                            }
                        }
                    }
                });
                // holder.item_iv_src.setImageBitmap(mBitmaps.get(position)); //将图片显示到ImageView中
                if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_LOCAL) {//只有本地app中的的图片视频才需要处理的事情
                    Glide.with(mContext)
                            .load(mRTPhotos.get(position).getmThumbnailPath())
                            .into(holder.item_iv_src);//直接加载图片
                    *//*if (mRTPhotos.get(position).isVisibility()) {
                        holder.item_iv_select.setVisibility(View.VISIBLE);
                    } else {
                        holder.item_iv_select.setVisibility(View.GONE);
                    }
                    if (mRTPhotos.get(position).isSelect()) {
                        holder.item_iv_select.setImageResource(R.mipmap.select_icon);
                    } else {
                        holder.item_iv_select.setImageResource(R.mipmap.select_not_icon);
                    }*//*
                }
                if (mRTPhotos.get(position).getFileType() == PHOTO_TYPE_VIDEO) {//只有视频才显示按钮
                    holder.item_iv_video.setVisibility(View.VISIBLE);
                }
            } else {
                Log.i(TAG, "onBindViewHolder:02 position=" + position);
                if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_SD) {
                    Glide.with(mContext)
                            .load(mRTPhotos.get(position).getFilePath())
                            .into(holder.item_iv_src);
                } else if (mRTPhotos.get(position).getGallerSrc() == PHOTO_SRC_LOCAL) {
                    if (mRTPhotos.get(position).isVisibility()) {
                        holder.item_iv_select.setVisibility(View.VISIBLE);
                    } else {
                        holder.item_iv_select.setVisibility(View.GONE);
                    }
                    if (mRTPhotos.get(position).isSelect()) {
                        holder.item_iv_select.setImageResource(R.drawable.select_icon);
                    } else {
                        holder.item_iv_select.setImageResource(R.drawable.select_not_icon);
                    }
                }
            }*/
        }

        @Override
        public void onBindViewHolder(@NonNull ViewHolder holder, int position) { }

        @Override
        public int getItemCount() {
            return mRTPhotos == null ? 0 : mRTPhotos.size();
        }


        class ViewHolder extends RecyclerView.ViewHolder {
            ImageView item_iv_src;
            ImageView item_iv_select;
            ImageView item_iv_video;

            ViewHolder(View itemView) {
                super(itemView);
                item_iv_src = itemView.findViewById(R.id.item_iv_src);
                item_iv_select = itemView.findViewById(R.id.item_iv_select);
                item_iv_video = itemView.findViewById(R.id.item_iv_video);
                ViewGroup.LayoutParams parm = item_iv_src.getLayoutParams(); //获取button背景的LayoutParams实例
                parm.width = mGLM.getWidth() / mGLM.getSpanCount();//RecyclerView宽度/一行中Item个数=单个Item宽度
                parm.height = parm.width * 9 / 16;
                item_iv_src.setLayoutParams(parm);
            }
        }
    }

    public void setOnClickItemListener(onClickItemListener onClickItemViewListener) {
        mOnClickItemViewListener = onClickItemViewListener;
    }

    public interface onClickItemListener {
        void onRecyclerViewItemClick(View view, ArrayList<JPhoto> RTPhotos, int position);

        void onRecyclerViewItemOnLongClick(View view, int position);
    }
}
