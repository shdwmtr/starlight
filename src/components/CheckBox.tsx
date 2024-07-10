import React, { ReactNode, useState } from 'react';

interface CustomCheckboxProps {
    children: ReactNode;
    isChecked?: boolean;
    onChange?: (isChecked: boolean) => void;
}

const CustomCheckbox: React.FC<CustomCheckboxProps> = ({ children, isChecked, onChange }) => {
  const [checked, setChecked] = useState(isChecked || false);

  const toggleCheckbox = () => {
    const newValue = !checked;
    setChecked(newValue);
    if (onChange) {
      onChange(newValue);
    }
  };

  return (
    <label className="custom-checkbox">
      <input
        type="checkbox"
        checked={checked}
        onChange={toggleCheckbox}
      />
      <span className="checkmark"></span>
      {children}
    </label>
  );
};

export default CustomCheckbox;