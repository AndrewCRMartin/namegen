data=$HOME/data/abnamedata
bin=$HOME/bin

function makedir
{
    dir=$1
    if [ ! -d $dir ]; then
        echo "Making directory: $dir"
        mkdir -p $dir
        if [ ! -d $dir ]; then
            echo "Failed to create directory: $dir"
            echo "...check permissions"
            exit 1
        fi
    fi
}

function CopyFile
{
    in=$1
    out=$2

    cp $in $out
    if [ -d $out ]; then
        if [ ! -e $out/`basename $in` ]; then
            echo "Failed to copy file $in to directory $out"
            exit 1;
        fi
    else
        if [ ! -e $out ];  then
            echo "Failed to copy file $in to file $out"
            exit 1;
        fi
    fi
}

# Create directories
makedir $bin
makedir $data

# Copy in the data files
for file in abnamedata/*
do
    CopyFile $file $data
done

# Copy in the executable
CopyFile abcheckname $bin

